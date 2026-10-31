#ifndef TRAIN_H
#define TRAIN_H

#include <stddef.h>
#include "booking.h"
#include "confirmed_list.h"
#include "waiting_queue.h"

#define MAX_TRAINS 10
#define MAX_SEATS_PER_TRAIN 100

/* Train structure */
typedef struct {
    int train_number;                /* Primary identifier for the train */
    char train_name[NAME_LEN];       /* Full descriptive name of the train */
    char source[ROUTE_LEN];          /* Origin station name */
    char destination[ROUTE_LEN];     /* Destination station name */
    char departure_time[TIME_LEN];   /* Scheduled departure time (e.g., "18:30") */
    int capacity;                    /* Total seat capacity (1 to MAX_SEATS_PER_TRAIN) */
    int seats[MAX_SEATS_PER_TRAIN];  /* Seat array: 0 = vacant, 1 = occupied */
    ConfirmedList confirmed;         /* Singly linked list of confirmed booking IDs */
    WaitingQueue waiting;            /* FIFO linked queue of waiting booking IDs */
} Train;

/* Train operations */
int train_init(Train *train, int train_number, const char *name, const char *source, const char *dest, const char *dep_time, int capacity);
void train_cleanup(Train *train);

/* Seat manager operations */
int seat_manager_find_first_vacant(const Train *train);
int seat_manager_allocate(Train *train, int seat_number);
int seat_manager_release(Train *train, int seat_number);
int seat_manager_is_seat_vacant(const Train *train, int seat_number);
int seat_manager_count_occupied(const Train *train);
int seat_manager_get_chart(const Train *train, int *seat_states_out, size_t max_output);

#endif /* TRAIN_H */
