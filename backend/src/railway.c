#include <string.h>
#include <stdio.h>
#include "railway.h"

int railway_init(RailwaySystem *system)
{
    if (system == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    memset(system, 0, sizeof(RailwaySystem));
    system->train_count = 0;
    system->booking_count = 0;
    system->event_count = 0;
    system->next_booking_id = 1001; /* Start booking IDs from 1001 */
    system->next_event_sequence = 1;

    return RAILWAY_SUCCESS;
}

void railway_cleanup(RailwaySystem *system)
{
    if (system == NULL) {
        return;
    }

    for (size_t i = 0; i < system->train_count; i++) {
        train_cleanup(&system->trains[i]);
    }

    system->train_count = 0;
    system->booking_count = 0;
    system->event_count = 0;
    system->next_booking_id = 1001;
    system->next_event_sequence = 1;
}

int railway_init_sample_trains(RailwaySystem *system)
{
    if (system == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    /* Initialize 3 sample trains with clear routes and manageable demonstration capacities */
    Train *t1 = &system->trains[0];
    train_init(t1, 101, "Mandovi Express", "Mumbai CSMT", "Madgaon", "07:10", 3);

    Train *t2 = &system->trains[1];
    train_init(t2, 102, "Konkan Kanya Express", "Mumbai CSMT", "Madgaon", "23:05", 4);

    Train *t3 = &system->trains[2];
    train_init(t3, 103, "Vande Bharat Express", "Mumbai CSMT", "Madgaon", "05:25", 5);

    system->train_count = 3;

    return RAILWAY_SUCCESS;
}

Train *railway_find_train(RailwaySystem *system, int train_number)
{
    if (system == NULL || train_number <= 0) {
        return NULL;
    }

    for (size_t i = 0; i < system->train_count; i++) {
        if (system->trains[i].train_number == train_number) {
            return &system->trains[i];
        }
    }

    return NULL;
}

const Train *railway_find_train_const(const RailwaySystem *system, int train_number)
{
    if (system == NULL || train_number <= 0) {
        return NULL;
    }

    for (size_t i = 0; i < system->train_count; i++) {
        if (system->trains[i].train_number == train_number) {
            return &system->trains[i];
        }
    }

    return NULL;
}

Booking *railway_find_booking(RailwaySystem *system, unsigned long booking_id)
{
    if (system == NULL || booking_id == 0) {
        return NULL;
    }

    for (size_t i = 0; i < system->booking_count; i++) {
        if (system->bookings[i].booking_id == booking_id) {
            return &system->bookings[i];
        }
    }

    return NULL;
}

const Booking *railway_find_booking_const(const RailwaySystem *system, unsigned long booking_id)
{
    if (system == NULL || booking_id == 0) {
        return NULL;
    }

    for (size_t i = 0; i < system->booking_count; i++) {
        if (system->bookings[i].booking_id == booking_id) {
            return &system->bookings[i];
        }
    }

    return NULL;
}

int railway_get_seat_chart(const RailwaySystem *system,
                           int train_number,
                           int *seat_states,
                           size_t output_capacity)
{
    if (system == NULL || seat_states == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    const Train *train = railway_find_train_const(system, train_number);
    if (train == NULL) {
        return RAILWAY_ERR_TRAIN_NOT_FOUND;
    }

    return seat_manager_get_chart(train, seat_states, output_capacity);
}

int railway_validate_invariants(const RailwaySystem *system)
{
    if (system == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    /* Invariant 1: Booking IDs in central registry must be unique */
    for (size_t i = 0; i < system->booking_count; i++) {
        for (size_t j = i + 1; j < system->booking_count; j++) {
            if (system->bookings[i].booking_id == system->bookings[j].booking_id) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }
        }
    }

    /* Invariant 2: Validate each train independently */
    for (size_t t = 0; t < system->train_count; t++) {
        const Train *train = &system->trains[t];

        if (train->capacity <= 0 || train->capacity > MAX_SEATS_PER_TRAIN) {
            return RAILWAY_ERR_INVARIANT_VIOLATION;
        }

        /* Verify confirmed list node count matches train->confirmed.size */
        size_t confirmed_node_count = 0;
        const ConfirmedNode *c_node = train->confirmed.head;
        while (c_node != NULL) {
            confirmed_node_count++;
            c_node = c_node->next;
        }

        if (confirmed_node_count != train->confirmed.size) {
            return RAILWAY_ERR_INVARIANT_VIOLATION;
        }

        /* Verify waiting queue pointer consistency and node count */
        if (train->waiting.size == 0) {
            if (train->waiting.front != NULL || train->waiting.rear != NULL) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }
        } else {
            if (train->waiting.front == NULL || train->waiting.rear == NULL) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }
            if (train->waiting.rear->next != NULL) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            size_t waiting_node_count = 0;
            const WaitingNode *w_node = train->waiting.front;
            while (w_node != NULL) {
                waiting_node_count++;
                w_node = w_node->next;
            }

            if (waiting_node_count != train->waiting.size) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }
        }

        /* Verify seat states: count occupied seats */
        int occupied_seats = 0;
        for (int s = 0; s < train->capacity; s++) {
            if (train->seats[s] == 1) {
                occupied_seats++;
            } else if (train->seats[s] != 0) {
                return RAILWAY_ERR_INVARIANT_VIOLATION; /* invalid seat state */
            }
        }

        /* Invariant: confirmed list size must equal occupied seats count */
        if ((size_t)occupied_seats != train->confirmed.size) {
            return RAILWAY_ERR_INVARIANT_VIOLATION;
        }

        /* Invariant: confirmed count + available seats == train capacity */
        int available_seats = train->capacity - occupied_seats;
        if (occupied_seats + available_seats != train->capacity) {
            return RAILWAY_ERR_INVARIANT_VIOLATION;
        }

        /* Array tracking seat assignments to guarantee no duplicate seat allocations */
        int seat_assigned[MAX_SEATS_PER_TRAIN] = {0};

        /* Invariant: Every node in confirmed list refers to an active CONFIRMED booking on THIS train */
        c_node = train->confirmed.head;
        while (c_node != NULL) {
            const Booking *b = railway_find_booking_const(system, c_node->booking_id);
            if (b == NULL) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->train_number != train->train_number) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->status != BOOKING_CONFIRMED) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->seat_number < 1 || b->seat_number > train->capacity) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            /* Check that train->seats[seat - 1] is marked occupied */
            if (train->seats[b->seat_number - 1] != 1) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            /* Check no duplicate seat assignment */
            if (seat_assigned[b->seat_number - 1] != 0) {
                return RAILWAY_ERR_INVARIANT_VIOLATION; /* duplicate seat assignment */
            }
            seat_assigned[b->seat_number - 1] = 1;

            c_node = c_node->next;
        }

        /* Invariant: Every node in waiting queue refers to a WAITING booking on THIS train with seat_number == 0 */
        const WaitingNode *w_node = train->waiting.front;
        while (w_node != NULL) {
            const Booking *b = railway_find_booking_const(system, w_node->booking_id);
            if (b == NULL) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->train_number != train->train_number) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->status != BOOKING_WAITING) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            if (b->seat_number != 0) {
                return RAILWAY_ERR_INVARIANT_VIOLATION; /* waiting booking must have seat 0 */
            }

            /* Invariant: Waiting passenger must not also be in confirmed list */
            if (confirmed_list_contains(&train->confirmed, b->booking_id)) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            w_node = w_node->next;
        }
    }

    /* Invariant 3: Cancelled bookings appear in neither confirmed list nor waiting queue */
    for (size_t i = 0; i < system->booking_count; i++) {
        const Booking *b = &system->bookings[i];
        if (b->status == BOOKING_CANCELLED) {
            if (b->seat_number != 0) {
                return RAILWAY_ERR_INVARIANT_VIOLATION;
            }

            const Train *train = railway_find_train_const(system, b->train_number);
            if (train != NULL) {
                if (confirmed_list_contains(&train->confirmed, b->booking_id)) {
                    return RAILWAY_ERR_INVARIANT_VIOLATION;
                }
                if (waiting_queue_contains(&train->waiting, b->booking_id)) {
                    return RAILWAY_ERR_INVARIANT_VIOLATION;
                }
            }
        }
    }

    return RAILWAY_SUCCESS;
}
