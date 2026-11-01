#include "railway.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_BUCKETS 101
#define FIRST_PNR 1000001

Train trains[TRAIN_COUNT];

static Passenger *pnr_table[HASH_BUCKETS];
static int next_pnr = FIRST_PNR;

/* ---- PNR hash table (separate chaining) ---- */

static void hash_insert(Passenger *p) {
    Passenger **bucket = &pnr_table[p->pnr % HASH_BUCKETS];
    p->hash_next = *bucket;
    *bucket = p;
}

static void hash_remove(Passenger *p) {
    Passenger **link = &pnr_table[p->pnr % HASH_BUCKETS];
    while (*link != p)
        link = &(*link)->hash_next;
    *link = p->hash_next;
}

Passenger *rail_find(int pnr) {
    if (pnr <= 0)
        return NULL;
    for (Passenger *p = pnr_table[pnr % HASH_BUCKETS]; p; p = p->hash_next)
        if (p->pnr == pnr)
            return p;
    return NULL;
}

/* ---- Waiting list (FIFO queue, doubly linked for removal from the middle) ---- */

static void wait_enqueue(Train *t, Passenger *p) {
    p->prev = t->wait_tail;
    p->next = NULL;
    if (t->wait_tail)
        t->wait_tail->next = p;
    else
        t->wait_head = p;
    t->wait_tail = p;
    t->wait_count++;
}

static void wait_remove(Train *t, Passenger *p) {
    if (p->prev)
        p->prev->next = p->next;
    else
        t->wait_head = p->next;
    if (p->next)
        p->next->prev = p->prev;
    else
        t->wait_tail = p->prev;
    p->prev = p->next = NULL;
    t->wait_count--;
}

int rail_wait_position(const Passenger *p) {
    int position = 1;
    for (const Passenger *q = p->prev; q; q = q->prev)
        position++;
    return position;
}

/* ---- Free seats (stack) ---- */

static void rebuild_free_seats(Train *t) {
    t->free_top = 0;
    /* Pushed in reverse so that the lowest seat number is popped first. */
    for (int seat = SEATS_PER_TRAIN; seat >= 1; seat--)
        if (!t->seats[seat - 1])
            t->free_seats[t->free_top++] = seat;
}

/* ---- Bookings ---- */

void rail_init(void) {
    static const char *const schedule[TRAIN_COUNT][6] = {
        {"12101", "Coastal Express", "Mumbai", "Goa", "06:10", "15:45"},
        {"12627", "Deccan Mail", "Pune", "Bengaluru", "20:30", "11:05"},
        {"12951", "Northern Star", "Delhi", "Jaipur", "07:05", "12:20"},
    };

    for (int i = 0; i < HASH_BUCKETS; i++) {
        while (pnr_table[i]) {
            Passenger *p = pnr_table[i];
            pnr_table[i] = p->hash_next;
            free(p);
        }
    }
    for (int i = 0; i < TRAIN_COUNT; i++) {
        Train *t = &trains[i];
        memset(t, 0, sizeof *t);
        t->number = schedule[i][0];
        t->name = schedule[i][1];
        t->from = schedule[i][2];
        t->to = schedule[i][3];
        t->departs = schedule[i][4];
        t->arrives = schedule[i][5];
        rebuild_free_seats(t);
    }
    next_pnr = FIRST_PNR;
}

static int valid_name(const char *name) {
    size_t len = strlen(name);
    if (len == 0 || len > PASSENGER_NAME_LEN || name[0] == ' ' || name[len - 1] == ' ')
        return 0;
    /* Bytes >= 0x80 are UTF-8 letters. Quotes and control characters never pass,
     * so a stored name is always safe inside the JSON and the records file. */
    for (const unsigned char *c = (const unsigned char *)name; *c; c++)
        if (!(isalpha(*c) || *c >= 0x80 || *c == ' ' || *c == '.' || *c == '-' || *c == '\''))
            return 0;
    return 1;
}

static Passenger *new_passenger(int pnr, int train, const char *name, int age, char gender) {
    Passenger *p = calloc(1, sizeof *p);
    if (!p)
        return NULL;
    p->pnr = pnr;
    p->train = train;
    p->age = age;
    p->gender = gender;
    strcpy(p->name, name);
    hash_insert(p);
    return p;
}

static void assign_seat(Train *t, Passenger *p, int seat) {
    p->status = CONFIRMED;
    p->seat = seat;
    t->seats[seat - 1] = p;
}

const char *rail_book(int train, const char *name, int age, char gender, Passenger **out) {
    if (train < 0 || train >= TRAIN_COUNT)
        return "Unknown train";
    if (!valid_name(name))
        return "Name must be 1-40 letters; spaces, dots, hyphens and apostrophes are allowed";
    if (age < 1 || age > 120)
        return "Age must be between 1 and 120";
    if (gender != 'M' && gender != 'F' && gender != 'O')
        return "Gender must be M, F or O";

    Train *t = &trains[train];
    if (t->free_top == 0 && t->wait_count == WAITLIST_LIMIT)
        return "This train and its waiting list are full";

    Passenger *p = new_passenger(next_pnr, train, name, age, gender);
    if (!p)
        return "Out of memory";
    next_pnr++;

    if (t->free_top > 0) {
        assign_seat(t, p, t->free_seats[--t->free_top]);
    } else {
        p->status = WAITING;
        wait_enqueue(t, p);
    }
    *out = p;
    return NULL;
}

int rail_cancel(int pnr, Passenger **promoted) {
    *promoted = NULL;
    Passenger *p = rail_find(pnr);
    if (!p)
        return -1;

    Train *t = &trains[p->train];
    if (p->status == WAITING) {
        wait_remove(t, p);
    } else {
        int seat = p->seat;
        t->seats[seat - 1] = NULL;
        if (t->wait_head) {
            Passenger *first = t->wait_head;
            wait_remove(t, first);
            assign_seat(t, first, seat);
            *promoted = first;
        } else {
            t->free_seats[t->free_top++] = seat;
        }
    }
    hash_remove(p);
    free(p);
    return 0;
}

/* ---- Records file ----
 * First line: next PNR. Then one line per passenger:
 *   pnr,train,status,seat,age,gender,name
 * Waiting passengers are written in queue order, which loading relies on. */

static void write_passenger(FILE *f, const Passenger *p) {
    fprintf(f, "%d,%d,%c,%d,%d,%c,%s\n", p->pnr, p->train, p->status == CONFIRMED ? 'C' : 'W',
            p->seat, p->age, p->gender, p->name);
}

int rail_save(const char *path) {
    char tmp_path[512];
    snprintf(tmp_path, sizeof tmp_path, "%s.tmp", path);

    FILE *f = fopen(tmp_path, "w");
    if (!f)
        return -1;
    fprintf(f, "%d\n", next_pnr);
    for (int i = 0; i < TRAIN_COUNT; i++) {
        for (int s = 0; s < SEATS_PER_TRAIN; s++)
            if (trains[i].seats[s])
                write_passenger(f, trains[i].seats[s]);
        for (Passenger *p = trains[i].wait_head; p; p = p->next)
            write_passenger(f, p);
    }
    if (fclose(f) != 0)
        return -1;
    /* Rename so a crash mid-write cannot damage the existing records. */
    return rename(tmp_path, path) == 0 ? 0 : -1;
}

static int load_line(const char *line) {
    int pnr, train, seat, age;
    char status, gender, name[PASSENGER_NAME_LEN + 1];

    if (sscanf(line, "%d,%d,%c,%d,%d,%c,%40[^\n]", &pnr, &train, &status, &seat, &age, &gender,
               name) != 7)
        return -1;
    if (pnr <= 0 || train < 0 || train >= TRAIN_COUNT || !valid_name(name) || rail_find(pnr))
        return -1;

    Train *t = &trains[train];
    if (status == 'C') {
        if (seat < 1 || seat > SEATS_PER_TRAIN || t->seats[seat - 1])
            return -1;
        assign_seat(t, new_passenger(pnr, train, name, age, gender), seat);
    } else if (status == 'W') {
        Passenger *p = new_passenger(pnr, train, name, age, gender);
        p->status = WAITING;
        wait_enqueue(t, p);
    } else {
        return -1;
    }
    return 0;
}

int rail_load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f)
        return 0; /* first run */

    int saved_next_pnr;
    char line[128];
    int ok = fgets(line, sizeof line, f) && sscanf(line, "%d", &saved_next_pnr) == 1;
    while (ok && fgets(line, sizeof line, f))
        ok = load_line(line) == 0;
    fclose(f);
    if (!ok) {
        rail_init();
        return -1;
    }

    next_pnr = saved_next_pnr;
    for (int i = 0; i < TRAIN_COUNT; i++)
        rebuild_free_seats(&trains[i]);
    return 0;
}
