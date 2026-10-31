#include <string.h>
#include <stdio.h>
#include "railway.h"
#include "booking.h"
#include "train.h"
#include "waiting_queue.h"
#include "confirmed_list.h"
#include "booking_history.h"

int railway_cancel_ticket(RailwaySystem *system,
                          unsigned long booking_id,
                          Booking *cancelled_result,
                          Booking *promoted_result)
{
    if (system == NULL || booking_id == 0) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    /* Locate booking in central registry */
    Booking *booking = railway_find_booking(system, booking_id);
    if (booking == NULL) {
        return RAILWAY_ERR_BOOKING_NOT_FOUND;
    }

    /* Reject repeated cancellation */
    if (booking->status == BOOKING_CANCELLED) {
        return RAILWAY_ERR_ALREADY_CANCELLED;
    }

    /* Only active confirmed tickets can be cancelled through standard cancellation */
    if (booking->status != BOOKING_CONFIRMED) {
        return RAILWAY_ERR_NOT_CONFIRMED;
    }

    /* Locate associated train */
    Train *train = railway_find_train(system, booking->train_number);
    if (train == NULL) {
        return RAILWAY_ERR_TRAIN_NOT_FOUND;
    }

    int freed_seat = booking->seat_number;
    if (freed_seat < 1 || freed_seat > train->capacity) {
        return RAILWAY_ERR_INVARIANT_VIOLATION;
    }

    /* Remove from confirmed list */
    int remove_res = confirmed_list_remove(&train->confirmed, booking_id);
    if (remove_res != RAILWAY_SUCCESS) {
        return remove_res;
    }

    /* Release seat in train seat array */
    seat_manager_release(train, freed_seat);

    /* Update central booking registry */
    booking->status = BOOKING_CANCELLED;
    booking->seat_number = 0;

    booking_history_record_event(system->events,
                                 &system->event_count,
                                 MAX_EVENTS,
                                 &system->next_event_sequence,
                                 "TICKET_CANCELLED",
                                 booking_id,
                                 train->train_number);

    if (cancelled_result != NULL) {
        *cancelled_result = *booking;
    }

    /* Check train's waiting queue for automatic FIFO promotion */
    if (!waiting_queue_is_empty(&train->waiting)) {
        unsigned long promoted_id = 0;
        int deq_res = waiting_queue_dequeue(&train->waiting, &promoted_id);

        if (deq_res == RAILWAY_SUCCESS && promoted_id > 0) {
            Booking *promoted_booking = railway_find_booking(system, promoted_id);

            if (promoted_booking != NULL && promoted_booking->train_number == train->train_number) {
                /* Allocate released seat to promoted passenger */
                seat_manager_allocate(train, freed_seat);
                promoted_booking->seat_number = freed_seat;
                promoted_booking->status = BOOKING_CONFIRMED;

                confirmed_list_insert(&train->confirmed, promoted_id);

                booking_history_record_event(system->events,
                                             &system->event_count,
                                             MAX_EVENTS,
                                             &system->next_event_sequence,
                                             "PROMOTED",
                                             promoted_id,
                                             train->train_number);

                if (promoted_result != NULL) {
                    *promoted_result = *promoted_booking;
                }
            }
        }
    } else {
        /* No passenger waiting: seat remains vacant (0) */
        if (promoted_result != NULL) {
            memset(promoted_result, 0, sizeof(Booking));
            promoted_result->booking_id = 0;
            promoted_result->seat_number = 0;
        }
    }

    return RAILWAY_SUCCESS;
}
