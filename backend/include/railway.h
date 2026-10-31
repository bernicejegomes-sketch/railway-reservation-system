#ifndef RAILWAY_H
#define RAILWAY_H

#include <stddef.h>
#include "booking.h"
#include "confirmed_list.h"
#include "waiting_queue.h"
#include "train.h"
#include "booking_history.h"
#include "validation.h"

/* Standardized return codes across all C core operations */
typedef enum {
    RAILWAY_SUCCESS                 =  0,
    RAILWAY_ERR_INVALID_PARAM       = -1,
    RAILWAY_ERR_TRAIN_NOT_FOUND     = -2,
    RAILWAY_ERR_BOOKING_NOT_FOUND   = -3,
    RAILWAY_ERR_CAPACITY_FULL       = -4,
    RAILWAY_ERR_ALREADY_CANCELLED   = -5,
    RAILWAY_ERR_NOT_CONFIRMED       = -6,
    RAILWAY_ERR_ALLOCATION_FAILED   = -7,
    RAILWAY_ERR_INVARIANT_VIOLATION = -8
} RailwayResult;

/* System root state context */
typedef struct {
    Train trains[MAX_TRAINS];
    size_t train_count;

    Booking bookings[MAX_BOOKINGS]; /* Central registry: active, waiting, and cancelled */
    size_t booking_count;

    BookingEvent events[MAX_EVENTS];
    size_t event_count;

    unsigned long next_booking_id;
    unsigned long next_event_sequence;
} RailwaySystem;

/* Core system lifecycle */
int railway_init(RailwaySystem *system);
void railway_cleanup(RailwaySystem *system);
int railway_init_sample_trains(RailwaySystem *system);

/* Lookups */
Train *railway_find_train(RailwaySystem *system, int train_number);
const Train *railway_find_train_const(const RailwaySystem *system, int train_number);

Booking *railway_find_booking(RailwaySystem *system, unsigned long booking_id);
const Booking *railway_find_booking_const(const RailwaySystem *system, unsigned long booking_id);

/* Core reservation operations */
int railway_book_ticket(RailwaySystem *system,
                        const char *passenger_name,
                        int age,
                        int train_number,
                        const char *journey_date,
                        Booking *result);

int railway_cancel_ticket(RailwaySystem *system,
                          unsigned long booking_id,
                          Booking *cancelled_result,
                          Booking *promoted_result);

int railway_get_seat_chart(const RailwaySystem *system,
                           int train_number,
                           int *seat_states,
                           size_t output_capacity);

/* Integrity and invariant verification */
int railway_validate_invariants(const RailwaySystem *system);

#endif /* RAILWAY_H */
