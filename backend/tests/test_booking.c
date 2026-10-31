#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "railway.h"

int main(void)
{
    printf("Running Booking Tests...\n");

    RailwaySystem system;
    assert(railway_init(&system) == RAILWAY_SUCCESS);
    assert(railway_init_sample_trains(&system) == RAILWAY_SUCCESS);
    /* Train 101 has capacity 3 */

    /* Test 1: Successful confirmed booking */
    Booking b1;
    int res = railway_book_ticket(&system, "Alice Wonder", 25, 101, "2026-11-20", &b1);
    assert(res == RAILWAY_SUCCESS);
    assert(b1.booking_id == 1001);
    assert(b1.status == BOOKING_CONFIRMED);
    assert(b1.seat_number == 1);
    assert(system.booking_count == 1);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Test 2: Sequential booking on same train */
    Booking b2;
    res = railway_book_ticket(&system, "Bob Builder", 32, 101, "2026-11-20", &b2);
    assert(res == RAILWAY_SUCCESS);
    assert(b2.booking_id == 1002);
    assert(b2.status == BOOKING_CONFIRMED);
    assert(b2.seat_number == 2);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    Booking b3;
    res = railway_book_ticket(&system, "Charlie Brown", 45, 101, "2026-11-20", &b3);
    assert(res == RAILWAY_SUCCESS);
    assert(b3.booking_id == 1003);
    assert(b3.status == BOOKING_CONFIRMED);
    assert(b3.seat_number == 3);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Train 101 is now full (3/3 seats occupied) */
    Train *t101 = railway_find_train(&system, 101);
    assert(t101 != NULL);
    assert(seat_manager_count_occupied(t101) == 3);
    assert(t101->confirmed.size == 3);
    assert(waiting_queue_is_empty(&t101->waiting) == 1);

    /* Test 3: Waiting list transition when train is full */
    Booking b4;
    res = railway_book_ticket(&system, "Diana Prince", 29, 101, "2026-11-20", &b4);
    assert(res == RAILWAY_SUCCESS);
    assert(b4.booking_id == 1004);
    assert(b4.status == BOOKING_WAITING);
    assert(b4.seat_number == 0); /* Must have seat 0 */
    assert(t101->waiting.size == 1);
    assert(waiting_queue_is_empty(&t101->waiting) == 0);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    Booking b5;
    res = railway_book_ticket(&system, "Evan Wright", 50, 101, "2026-11-20", &b5);
    assert(res == RAILWAY_SUCCESS);
    assert(b5.booking_id == 1005);
    assert(b5.status == BOOKING_WAITING);
    assert(b5.seat_number == 0);
    assert(t101->waiting.size == 2);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Test 4: Rejection of invalid inputs */
    Booking err_booking;
    /* Empty name */
    assert(railway_book_ticket(&system, "", 25, 101, "2026-11-20", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    /* Whitespace name */
    assert(railway_book_ticket(&system, "   ", 25, 101, "2026-11-20", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    /* NULL name */
    assert(railway_book_ticket(&system, NULL, 25, 101, "2026-11-20", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    /* Invalid age */
    assert(railway_book_ticket(&system, "Valid Name", 0, 101, "2026-11-20", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    assert(railway_book_ticket(&system, "Valid Name", 150, 101, "2026-11-20", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    /* Invalid date format */
    assert(railway_book_ticket(&system, "Valid Name", 30, 101, "20-11-2026", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    assert(railway_book_ticket(&system, "Valid Name", 30, 101, "invalid-date", &err_booking) == RAILWAY_ERR_INVALID_PARAM);
    /* Unknown train number */
    assert(railway_book_ticket(&system, "Valid Name", 30, 999, "2026-11-20", &err_booking) == RAILWAY_ERR_TRAIN_NOT_FOUND);

    /* Test 5: Verify lookup by booking ID */
    Booking *found = railway_find_booking(&system, 1001);
    assert(found != NULL);
    assert(strcmp(found->passenger_name, "Alice Wonder") == 0);
    assert(found->status == BOOKING_CONFIRMED);
    assert(found->seat_number == 1);

    Booking *not_found = railway_find_booking(&system, 9999);
    assert(not_found == NULL);

    railway_cleanup(&system);
    printf("Booking Tests PASSED.\n");
    return 0;
}
