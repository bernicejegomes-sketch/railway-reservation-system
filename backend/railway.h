#ifndef RAILWAY_H
#define RAILWAY_H

#define TRAIN_COUNT 3
#define SEATS_PER_TRAIN 12
#define WAITLIST_LIMIT 6
#define PASSENGER_NAME_LEN 40

typedef enum { CONFIRMED, WAITING } Status;

typedef struct Passenger {
    int pnr;
    char name[PASSENGER_NAME_LEN + 1];
    int age;
    char gender; /* 'M', 'F' or 'O' */
    int train;   /* index into trains[] */
    Status status;
    int seat;    /* 1-based seat number; 0 while waiting */

    struct Passenger *prev, *next; /* waiting list links */
    struct Passenger *hash_next;   /* PNR hash table chain */
} Passenger;

typedef struct {
    const char *number, *name, *from, *to, *departs, *arrives;

    Passenger *seats[SEATS_PER_TRAIN]; /* seat number - 1 -> passenger, or NULL */

    int free_seats[SEATS_PER_TRAIN]; /* stack of free seat numbers */
    int free_top;                    /* number of items on the stack */

    Passenger *wait_head, *wait_tail; /* FIFO queue as a doubly linked list */
    int wait_count;
} Train;

extern Train trains[TRAIN_COUNT];

/* Clears all bookings and restores empty trains. */
void rail_init(void);

/* Books a seat, or a waiting-list place if the train is full.
 * Returns NULL and sets *out on success, or an error message on rejection. */
const char *rail_book(int train, const char *name, int age, char gender, Passenger **out);

/* Returns the passenger with this PNR, or NULL. */
Passenger *rail_find(int pnr);

/* Cancels a booking. Returns 0, or -1 if the PNR is unknown.
 * *promoted is set to the passenger moved off the waiting list, or NULL. */
int rail_cancel(int pnr, Passenger **promoted);

/* Returns a waiting passenger's 1-based place in the queue. */
int rail_wait_position(const Passenger *p);

/* Records are stored as text. Both return 0, or -1 on failure.
 * Loading a file that does not exist is not a failure. */
int rail_save(const char *path);
int rail_load(const char *path);

#endif
