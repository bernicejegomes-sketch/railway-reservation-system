#include <stdio.h>
#include <assert.h>
#include "train.h"
#include "railway.h"

int main(void)
{
    printf("Running Seat Manager Tests...\n");

    Train train;
    int res = train_init(&train, 201, "Test Shuttle", "Station A", "Station B", "12:00", 3);
    assert(res == RAILWAY_SUCCESS);
    assert(train.capacity == 3);

    /* Test 1: First available seat on empty train */
    assert(seat_manager_find_first_vacant(&train) == 1);
    assert(seat_manager_count_occupied(&train) == 0);
    assert(seat_manager_is_seat_vacant(&train, 1) == 1);
    assert(seat_manager_is_seat_vacant(&train, 2) == 1);
    assert(seat_manager_is_seat_vacant(&train, 3) == 1);

    /* Test 2: Sequential allocation */
    assert(seat_manager_allocate(&train, 1) == RAILWAY_SUCCESS);
    assert(train.seats[0] == 1);
    assert(seat_manager_count_occupied(&train) == 1);
    assert(seat_manager_find_first_vacant(&train) == 2);

    /* Test 3: Duplicate allocation rejection */
    assert(seat_manager_allocate(&train, 1) == RAILWAY_ERR_CAPACITY_FULL);

    /* Test 4: Allocate remaining seats */
    assert(seat_manager_allocate(&train, 2) == RAILWAY_SUCCESS);
    assert(seat_manager_allocate(&train, 3) == RAILWAY_SUCCESS);
    assert(seat_manager_count_occupied(&train) == 3);

    /* Test 5: Full train condition */
    assert(seat_manager_find_first_vacant(&train) == 0);
    assert(seat_manager_allocate(&train, 1) == RAILWAY_ERR_CAPACITY_FULL);

    /* Test 6: Seat release and lowest-seat reuse */
    assert(seat_manager_release(&train, 2) == RAILWAY_SUCCESS);
    assert(train.seats[1] == 0);
    assert(seat_manager_count_occupied(&train) == 2);
    /* Seat 2 is now vacant, so find_first_vacant should return 2 */
    assert(seat_manager_find_first_vacant(&train) == 2);

    /* Release seat 1 as well */
    assert(seat_manager_release(&train, 1) == RAILWAY_SUCCESS);
    assert(seat_manager_count_occupied(&train) == 1);
    /* Lowest vacant seat is now 1 */
    assert(seat_manager_find_first_vacant(&train) == 1);

    /* Reallocate seat 1 */
    assert(seat_manager_allocate(&train, 1) == RAILWAY_SUCCESS);
    assert(seat_manager_find_first_vacant(&train) == 2);

    /* Test 7: Out-of-bounds seat handling */
    assert(seat_manager_allocate(&train, 0) == RAILWAY_ERR_INVALID_PARAM);
    assert(seat_manager_allocate(&train, 4) == RAILWAY_ERR_INVALID_PARAM);
    assert(seat_manager_release(&train, 0) == RAILWAY_ERR_INVALID_PARAM);
    assert(seat_manager_release(&train, 5) == RAILWAY_ERR_INVALID_PARAM);

    /* Test 8: Seat chart extraction */
    int chart[3] = {0};
    int copied = seat_manager_get_chart(&train, chart, 3);
    assert(copied == 3);
    assert(chart[0] == 1); /* Seat 1 occupied */
    assert(chart[1] == 0); /* Seat 2 vacant */
    assert(chart[2] == 1); /* Seat 3 occupied */

    train_cleanup(&train);
    printf("Seat Manager Tests PASSED.\n");
    return 0;
}
