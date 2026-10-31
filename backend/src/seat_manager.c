#include <string.h>
#include "train.h"
#include "railway.h"

int seat_manager_find_first_vacant(const Train *train)
{
    if (train == NULL || train->capacity <= 0 || train->capacity > MAX_SEATS_PER_TRAIN) {
        return 0;
    }

    for (int i = 0; i < train->capacity; i++) {
        if (train->seats[i] == 0) {
            return i + 1; /* 1-based seat number */
        }
    }

    return 0; /* All seats occupied */
}

int seat_manager_allocate(Train *train, int seat_number)
{
    if (train == NULL || seat_number < 1 || seat_number > train->capacity) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    int idx = seat_number - 1;
    if (train->seats[idx] != 0) {
        return RAILWAY_ERR_CAPACITY_FULL; /* Already occupied */
    }

    train->seats[idx] = 1;
    return RAILWAY_SUCCESS;
}

int seat_manager_release(Train *train, int seat_number)
{
    if (train == NULL || seat_number < 1 || seat_number > train->capacity) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    int idx = seat_number - 1;
    train->seats[idx] = 0;
    return RAILWAY_SUCCESS;
}

int seat_manager_is_seat_vacant(const Train *train, int seat_number)
{
    if (train == NULL || seat_number < 1 || seat_number > train->capacity) {
        return 0;
    }

    return (train->seats[seat_number - 1] == 0);
}

int seat_manager_count_occupied(const Train *train)
{
    if (train == NULL || train->capacity <= 0) {
        return 0;
    }

    int count = 0;
    int limit = (train->capacity > MAX_SEATS_PER_TRAIN) ? MAX_SEATS_PER_TRAIN : train->capacity;

    for (int i = 0; i < limit; i++) {
        if (train->seats[i] == 1) {
            count++;
        }
    }

    return count;
}

int seat_manager_get_chart(const Train *train, int *seat_states_out, size_t max_output)
{
    if (train == NULL || seat_states_out == NULL || train->capacity <= 0) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    size_t copy_count = (size_t)train->capacity;
    if (copy_count > max_output) {
        copy_count = max_output;
    }

    for (size_t i = 0; i < copy_count; i++) {
        seat_states_out[i] = train->seats[i];
    }

    return (int)copy_count;
}
