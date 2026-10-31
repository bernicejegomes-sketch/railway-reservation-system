#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "railway.h"

int main(void)
{
    printf("Running Cancellation and Automatic Promotion Tests...\n");

    RailwaySystem system;
    assert(railway_init(&system) == RAILWAY_SUCCESS);
    assert(railway_init_sample_trains(&system) == RAILWAY_SUCCESS);
    /* Train 101 capacity = 3, Train 102 capacity = 4 */

    /* Scenario 1: Book 3 confirmed tickets on Train 101 (fills train) */
    Booking b1, b2, b3;
    assert(railway_book_ticket(&system, "Alice", 25, 101, "2026-11-20", &b1) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Bob", 30, 101, "2026-11-20", &b2) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Charlie", 35, 101, "2026-11-20", &b3) == RAILWAY_SUCCESS);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Scenario 2: Cancellation with EMPTY waiting queue */
    Booking cancelled, promoted;
    int res = railway_cancel_ticket(&system, b2.booking_id, &cancelled, &promoted);
    assert(res == RAILWAY_SUCCESS);
    assert(cancelled.booking_id == b2.booking_id);
    assert(cancelled.status == BOOKING_CANCELLED);
    assert(cancelled.seat_number == 0);
    /* No passenger was waiting, so promoted.booking_id must be 0 */
    assert(promoted.booking_id == 0);

    /* Verify seat 2 is now vacant */
    Train *t101 = railway_find_train(&system, 101);
    assert(t101 != NULL);
    assert(t101->seats[1] == 0);
    assert(seat_manager_count_occupied(t101) == 2);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Scenario 3: Reject repeated cancellation */
    assert(railway_cancel_ticket(&system, b2.booking_id, &cancelled, &promoted) == RAILWAY_ERR_ALREADY_CANCELLED);

    /* Scenario 4: Reject cancellation of non-existent booking ID */
    assert(railway_cancel_ticket(&system, 9999, &cancelled, &promoted) == RAILWAY_ERR_BOOKING_NOT_FOUND);

    /* Re-book seat 2 with Daniel */
    Booking b4;
    assert(railway_book_ticket(&system, "Daniel", 28, 101, "2026-11-20", &b4) == RAILWAY_SUCCESS);
    assert(b4.seat_number == 2); /* Must reuse the freed seat 2 */
    assert(b4.status == BOOKING_CONFIRMED);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Enqueue 2 waiting passengers on Train 101: Eva (WL 1) and Frank (WL 2) */
    Booking b5, b6;
    assert(railway_book_ticket(&system, "Eva", 22, 101, "2026-11-20", &b5) == RAILWAY_SUCCESS);
    assert(b5.status == BOOKING_WAITING);
    assert(b5.seat_number == 0);

    assert(railway_book_ticket(&system, "Frank", 40, 101, "2026-11-20", &b6) == RAILWAY_SUCCESS);
    assert(b6.status == BOOKING_WAITING);
    assert(b6.seat_number == 0);

    assert(t101->waiting.size == 2);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Scenario 5: Cancel Daniel (seat 2); verify Eva (earliest waiting) is promoted to seat 2 */
    res = railway_cancel_ticket(&system, b4.booking_id, &cancelled, &promoted);
    assert(res == RAILWAY_SUCCESS);
    assert(cancelled.booking_id == b4.booking_id);
    assert(cancelled.status == BOOKING_CANCELLED);

    /* Automatic promotion assertions */
    assert(promoted.booking_id == b5.booking_id); /* Eva must be promoted */
    assert(promoted.status == BOOKING_CONFIRMED);
    assert(promoted.seat_number == 2); /* Inherited seat 2 */

    /* Verify registry record for Eva was updated directly */
    Booking *eva_reg = railway_find_booking(&system, b5.booking_id);
    assert(eva_reg != NULL);
    assert(eva_reg->status == BOOKING_CONFIRMED);
    assert(eva_reg->seat_number == 2);

    /* Frank should now be the sole passenger in waiting queue (WL position 1) */
    assert(t101->waiting.size == 1);
    unsigned long next_waiting_id = 0;
    assert(waiting_queue_peek(&t101->waiting, &next_waiting_id) == RAILWAY_SUCCESS);
    assert(next_waiting_id == b6.booking_id);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Cancel Alice (seat 1); verify Frank is promoted to seat 1 */
    res = railway_cancel_ticket(&system, b1.booking_id, &cancelled, &promoted);
    assert(res == RAILWAY_SUCCESS);
    assert(cancelled.booking_id == b1.booking_id);
    assert(promoted.booking_id == b6.booking_id); /* Frank promoted */
    assert(promoted.status == BOOKING_CONFIRMED);
    assert(promoted.seat_number == 1); /* Inherited seat 1 */
    assert(t101->waiting.size == 0);
    assert(waiting_queue_is_empty(&t101->waiting) == 1);
    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    /* Scenario 6: Multi-train isolation test */
    /* Fill Train 102 (capacity 4) and add a waiting passenger Grace */
    Booking t2_b1, t2_b2, t2_b3, t2_b4, t2_wait;
    assert(railway_book_ticket(&system, "George", 20, 102, "2026-11-20", &t2_b1) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Hannah", 21, 102, "2026-11-20", &t2_b2) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Ian", 22, 102, "2026-11-20", &t2_b3) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Jack", 23, 102, "2026-11-20", &t2_b4) == RAILWAY_SUCCESS);
    assert(railway_book_ticket(&system, "Grace", 24, 102, "2026-11-20", &t2_wait) == RAILWAY_SUCCESS);
    assert(t2_wait.status == BOOKING_WAITING);

    Train *t102 = railway_find_train(&system, 102);
    assert(t102->waiting.size == 1);

    /* Now cancel a booking on Train 101 (e.g. Charlie b3) */
    /* Grace is waiting on Train 102, so cancellation on Train 101 must NOT promote Grace */
    res = railway_cancel_ticket(&system, b3.booking_id, &cancelled, &promoted);
    assert(res == RAILWAY_SUCCESS);
    assert(cancelled.booking_id == b3.booking_id);
    assert(promoted.booking_id == 0); /* No promotion on Train 101 */

    /* Grace must still be waiting on Train 102 */
    Booking *grace_reg = railway_find_booking(&system, t2_wait.booking_id);
    assert(grace_reg != NULL);
    assert(grace_reg->status == BOOKING_WAITING);
    assert(grace_reg->seat_number == 0);
    assert(t102->waiting.size == 1);

    assert(railway_validate_invariants(&system) == RAILWAY_SUCCESS);

    railway_cleanup(&system);
    printf("Cancellation and Automatic Promotion Tests PASSED.\n");
    return 0;
}
