/* Minimal single-threaded HTTP server exposing the reservation system as a JSON API.
 * Requests are handled one at a time, so the data structures need no locking. */

#include "railway.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define REQUEST_MAX 8192
#define RESPONSE_MAX 32768
#define RECORDS_FILE "records.csv"
#define FRONTEND_FILE "frontend/index.html"

/* ---- Response building ---- */

static char response[RESPONSE_MAX];
static size_t response_len;

static void out(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(response + response_len, RESPONSE_MAX - response_len, format, args);
    va_end(args);
    if (n > 0 && (size_t)n < RESPONSE_MAX - response_len)
        response_len += (size_t)n;
}

static void send_all(int fd, const char *data, size_t len) {
    while (len > 0) {
        ssize_t n = send(fd, data, len, 0);
        if (n <= 0)
            return;
        data += n;
        len -= (size_t)n;
    }
}

static void send_response(int fd, int code, const char *type, const char *body, size_t len) {
    char header[256];
    int n = snprintf(header, sizeof header,
                     "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n"
                     "Connection: close\r\n\r\n",
                     code, code == 200 ? "OK" : "Error", type, len);
    send_all(fd, header, (size_t)n);
    send_all(fd, body, len);
}

static void send_json(int fd, int code) {
    send_response(fd, code, "application/json", response, response_len);
}

static void send_error(int fd, int code, const char *message) {
    response_len = 0;
    out("{\"error\":\"%s\"}", message);
    send_json(fd, code);
}

static void out_passenger(const Passenger *p) {
    out("{\"pnr\":%d,\"name\":\"%s\",\"age\":%d,\"gender\":\"%c\",\"train\":%d,"
        "\"status\":\"%s\",\"seat\":%d,\"position\":%d}",
        p->pnr, p->name, p->age, p->gender, p->train + 1,
        p->status == CONFIRMED ? "CONFIRMED" : "WAITING", p->seat,
        p->status == WAITING ? rail_wait_position(p) : 0);
}

static void out_state(void) {
    out("{\"seatsPerTrain\":%d,\"waitlistLimit\":%d,\"trains\":[", SEATS_PER_TRAIN, WAITLIST_LIMIT);
    for (int i = 0; i < TRAIN_COUNT; i++) {
        const Train *t = &trains[i];
        out("%s{\"id\":%d,\"number\":\"%s\",\"name\":\"%s\",\"from\":\"%s\",\"to\":\"%s\","
            "\"departs\":\"%s\",\"arrives\":\"%s\",\"free\":%d,\"passengers\":[",
            i ? "," : "", i + 1, t->number, t->name, t->from, t->to, t->departs, t->arrives,
            t->free_top);
        int first = 1;
        for (int s = 0; s < SEATS_PER_TRAIN; s++) {
            if (t->seats[s]) {
                if (!first)
                    out(",");
                out_passenger(t->seats[s]);
                first = 0;
            }
        }
        out("],\"waitlist\":[");
        for (const Passenger *p = t->wait_head; p; p = p->next) {
            out_passenger(p);
            if (p->next)
                out(",");
        }
        out("]}");
    }
    out("]}");
}

/* ---- Request parsing ---- */

static int hex_value(char c) {
    return isdigit((unsigned char)c) ? c - '0' : tolower((unsigned char)c) - 'a' + 10;
}

/* Copies the URL-decoded value of `key` from a "a=1&b=2" string into dest. */
static int form_get(const char *form, const char *key, char *dest, size_t dest_size) {
    size_t key_len = strlen(key);
    const char *p = form;

    while (*p) {
        const char *end = strchr(p, '&');
        if (!end)
            end = p + strlen(p);

        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            size_t n = 0;
            for (const char *c = p + key_len + 1; c < end; c++) {
                char decoded = *c;
                if (*c == '+') {
                    decoded = ' ';
                } else if (*c == '%') {
                    if (end - c < 3 || !isxdigit((unsigned char)c[1]) ||
                        !isxdigit((unsigned char)c[2]))
                        return 0;
                    decoded = (char)(hex_value(c[1]) * 16 + hex_value(c[2]));
                    c += 2;
                }
                if (decoded == '\0' || n + 1 >= dest_size)
                    return 0;
                dest[n++] = decoded;
            }
            dest[n] = '\0';
            return 1;
        }
        p = *end ? end + 1 : end;
    }
    return 0;
}

/* Reads a positive integer field; returns -1 if missing or malformed. */
static int form_get_int(const char *form, const char *key) {
    char value[16], *end;
    if (!form_get(form, key, value, sizeof value) || value[0] == '\0')
        return -1;
    long n = strtol(value, &end, 10);
    return (*end == '\0' && n > 0 && n <= 2000000000) ? (int)n : -1;
}

static long content_length(const char *request) {
    const char *line = strstr(request, "\r\n");
    while (line && strncmp(line, "\r\n\r\n", 4) != 0) {
        line += 2;
        if (strncasecmp(line, "Content-Length:", 15) == 0)
            return strtol(line + 15, NULL, 10);
        line = strstr(line, "\r\n");
    }
    return 0;
}

/* Reads a whole request into buffer. Returns the body pointer, or NULL if it is
 * malformed, too large or the client stalled. */
static char *read_request(int fd, char *buffer) {
    size_t total = 0;
    while (total < REQUEST_MAX) {
        ssize_t n = recv(fd, buffer + total, REQUEST_MAX - total, 0);
        if (n <= 0)
            return NULL;
        total += (size_t)n;
        buffer[total] = '\0';

        char *header_end = strstr(buffer, "\r\n\r\n");
        if (header_end) {
            char *body = header_end + 4;
            long length = content_length(buffer);
            if (length < 0 || length > REQUEST_MAX)
                return NULL;
            if (total - (size_t)(body - buffer) >= (size_t)length)
                return body;
        }
    }
    return NULL;
}

/* ---- Routes ---- */

static void persist(void) {
    if (rail_save(RECORDS_FILE) != 0)
        perror("could not save " RECORDS_FILE);
}

static void serve_frontend(int fd) {
    FILE *f = fopen(FRONTEND_FILE, "rb");
    if (!f) {
        send_error(fd, 500, "Frontend not found; run the server from the project root");
        return;
    }
    static char page[1 << 18];
    size_t len = fread(page, 1, sizeof page, f);
    fclose(f);
    send_response(fd, 200, "text/html; charset=utf-8", page, len);
}

static void handle_pnr(int fd, const char *query) {
    Passenger *p = rail_find(form_get_int(query, "pnr"));
    if (!p) {
        send_error(fd, 404, "No booking found for this PNR");
        return;
    }
    response_len = 0;
    out_passenger(p);
    send_json(fd, 200);
}

static void handle_book(int fd, const char *body) {
    char name[PASSENGER_NAME_LEN * 2], gender[4] = "";
    if (!form_get(body, "name", name, sizeof name))
        name[0] = '\0';
    form_get(body, "gender", gender, sizeof gender);

    Passenger *p;
    const char *error = rail_book(form_get_int(body, "train") - 1, name,
                                  form_get_int(body, "age"), gender[0], &p);
    if (error) {
        send_error(fd, 400, error);
        return;
    }
    persist();
    response_len = 0;
    out_passenger(p);
    send_json(fd, 200);
}

static void handle_cancel(int fd, const char *body) {
    int pnr = form_get_int(body, "pnr");
    Passenger *promoted;
    if (rail_cancel(pnr, &promoted) != 0) {
        send_error(fd, 404, "No booking found for this PNR");
        return;
    }
    persist();
    response_len = 0;
    out("{\"cancelled\":%d,\"promoted\":", pnr);
    if (promoted)
        out_passenger(promoted);
    else
        out("null");
    out("}");
    send_json(fd, 200);
}

static void handle_client(int fd) {
    static char request[REQUEST_MAX + 1];
    char method[8], target[256];

    char *body = read_request(fd, request);
    if (!body || sscanf(request, "%7s %255s", method, target) != 2) {
        send_error(fd, 400, "Bad request");
        return;
    }

    char *query = strchr(target, '?');
    if (query)
        *query++ = '\0';
    else
        query = "";

    int get = strcmp(method, "GET") == 0, post = strcmp(method, "POST") == 0;

    if (get && strcmp(target, "/") == 0) {
        serve_frontend(fd);
    } else if (get && strcmp(target, "/api/state") == 0) {
        response_len = 0;
        out_state();
        send_json(fd, 200);
    } else if (get && strcmp(target, "/api/pnr") == 0) {
        handle_pnr(fd, query);
    } else if (post && strcmp(target, "/api/book") == 0) {
        handle_book(fd, body);
    } else if (post && strcmp(target, "/api/cancel") == 0) {
        handle_cancel(fd, body);
    } else {
        send_error(fd, 404, "Not found");
    }
}

int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 8080;

    signal(SIGPIPE, SIG_IGN);
    rail_init();
    if (rail_load(RECORDS_FILE) != 0) {
        fprintf(stderr, RECORDS_FILE " is damaged; fix or remove it, then restart\n");
        return 1;
    }

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(server, (struct sockaddr *)&address, sizeof address) != 0 || listen(server, 16) != 0) {
        perror("could not start server");
        return 1;
    }
    printf("Railway reservation system running at http://localhost:%d\n", port);
    fflush(stdout);

    for (;;) {
        int client = accept(server, NULL, NULL);
        if (client < 0)
            continue;
        /* A stalled client must not block everyone else. */
        struct timeval timeout = {.tv_sec = 2};
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        handle_client(client);
        close(client);
    }
}
