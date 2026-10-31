#include <string.h>
#include <stdio.h>
#include "booking.h"
#include "railway.h"
#include "train.h"
#include "validation.h"
#include "booking_history.h"

int railway_book_ticket(RailwaySystem *system,
                        const char *passenger_name,
                        int age,
                        int train_number,
                        const char *journey_date,
                        Booking *result)
{
    if (system == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    /* Capacity check for central registry */
    if (system->booking_count >= MAX_BOOKINGS) {
        return RAILWAY_ERR_CAPACITY_FULL;
    }

    /* Validate passenger details */
    if (!validate_passenger_name(passenger_name)) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    if (!validate_age(age)) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    if (!validate_train_number(train_number)) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    if (!validate_journey_date(journey_date)) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    /* Locate the target train */
    Train *train = railway_find_train(system, train_number);
    if (train == NULL) {
        return RAILWAY_ERR_TRAIN_NOT_FOUND;
    }

    /* Generate new unique booking entry */
    unsigned long new_id = system->next_booking_id++;
    Booking *b = &system->bookings[system->booking_count];
    memset(b, 0, sizeof(Booking));

    b->booking_id = new_id;
    b->created_sequence = system->next_event_sequence;
    b->age = age;
    b->train_number = train_number;

    strncpy(b->passenger_name, passenger_name, NAME_LEN - 1);
    b->passenger_name[NAME_LEN - 1] = '\0';

    strncpy(b->journey_date, journey_date, DATE_LEN - 1);
    b->journey_date[DATE_LEN - 1] = '\0';

    /* Check seat availability in train */
    int vacant_seat = seat_manager_find_first_vacant(train);

    if (vacant_seat > 0) {
        /* Seats are available: assign the lowest-numbered vacant seat */
        int alloc_res = seat_manager_allocate(train, vacant_seat);
        if (alloc_res != RAILWAY_SUCCESS) {
            return alloc_res;
        }

        b->seat_number = vacant_seat;
        b->status = BOOKING_CONFIRMED;

        int list_res = confirmed_list_insert(&train->confirmed, b->booking_id);
        if (list_res != RAILWAY_SUCCESS) {
            /* Rollback seat allocation on failure */
            seat_manager_release(train, vacant_seat);
            return list_res;
        }

        booking_history_record_event(system->events,
                                     &system->event_count,
                                     MAX_EVENTS,
                                     &system->next_event_sequence,
                                     "BOOKING_CREATED",
                                     b->booking_id,
                                     train_number);
    } else {
        /* All physical seats occupied: place passenger into FIFO waiting queue */
        b->seat_number = 0;
        b->status = BOOKING_WAITING;

        int queue_res = waiting_queue_enqueue(&train->waiting, b->booking_id);
        if (queue_res != RAILWAY_SUCCESS) {
            return queue_res;
        }

        booking_history_record_event(system->events,
                                     &system->event_count,
                                     MAX_EVENTS,
                                     &system->next_event_sequence,
                                     "WAITING_JOINED",
                                     b->booking_id,
                                     train_number);
    }

    /* Persist in registry */
    system->booking_count++;

    if (result != NULL) {
        *result = *b;
    }

    return RAILWAY_SUCCESS;
}
