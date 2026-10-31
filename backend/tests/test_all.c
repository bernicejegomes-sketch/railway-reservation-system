#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "railway.h"

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define RUN_TEST(fn) do { \
    printf("[RUNNING] %s ... ", #fn); \
    g_tests_run++; \
    fn(); \
    g_tests_passed++; \
    printf("PASS\n"); \
} while (0)

/* 1. Waiting Queue Tests */
static void test_waiting_queue_suite(void)
{
    WaitingQueue q;
    assert(waiting_queue_init(&q) == RAILWAY_SUCCESS);
    assert(waiting_queue_is_empty(&q) == 1);
    assert(q.front == NULL && q.rear == NULL && q.size == 0);

    /* Enqueue single */
    assert(waiting_queue_enqueue(&q, 101) == RAILWAY_SUCCESS);
    assert(waiting_queue_is_empty(&q) == 0 && q.size == 1);
    assert(q.front == q.rear && q.front != NULL);

    /* Enqueue multiple */
    assert(waiting_queue_enqueue(&q, 102) == RAILWAY_SUCCESS);
    assert(waiting_queue_enqueue(&q, 103) == RAILWAY_SUCCESS);
    assert(q.size == 3);

    /* Peek */
    unsigned long id = 0;
    assert(waiting_queue_peek(&q, &id) == RAILWAY_SUCCESS && id == 101);

    /* FIFO Dequeue */
    assert(waiting_queue_dequeue(&q, &id) == RAILWAY_SUCCESS && id == 101);
    assert(waiting_queue_dequeue(&q, &id) == RAILWAY_SUCCESS && id == 102);
    assert(waiting_queue_dequeue(&q, &id) == RAILWAY_SUCCESS && id == 103);
    assert(waiting_queue_is_empty(&q) == 1);
    assert(q.front == NULL && q.rear == NULL && q.size == 0);

    waiting_queue_clear(&q);
}

/* 2. Confirmed List Tests */
static void test_confirmed_list_suite(void)
{
    ConfirmedList list;
    assert(confirmed_list_init(&list) == RAILWAY_SUCCESS);
    assert(list.head == NULL && list.size == 0);

    assert(confirmed_list_insert(&list, 501) == RAILWAY_SUCCESS);
    assert(confirmed_list_insert(&list, 502) == RAILWAY_SUCCESS);
    assert(confirmed_list_insert(&list, 503) == RAILWAY_SUCCESS);
    assert(list.size == 3);
    assert(confirmed_list_contains(&list, 502) == 1);

    /* Remove middle */
    assert(confirmed_list_remove(&list, 502) == RAILWAY_SUCCESS);
    assert(list.size == 2);
    assert(confirmed_list_contains(&list, 502) == 0);

    /* Remove head */
    assert(confirmed_list_remove(&list, 503) == RAILWAY_SUCCESS);
    assert(list.size == 1);

    /* Remove tail */
    assert(confirmed_list_remove(&list, 501) == RAILWAY_SUCCESS);
    assert(list.size == 0 && list.head == NULL);

    confirmed_list_clear(&list);
}

/* 3. Seat Manager Tests */
static void test_seat_manager_suite(void)
{
    Train train;
    assert(train_init(&train, 1, "Test Train", "Station A", "Station B", "09:00", 3) == RAILWAY_SUCCESS);
    assert(seat_manager_find_first_vacant(&train) == 1);

    assert(seat_manager_allocate(&train, 1) == RAILWAY_SUCCESS);
    assert(seat_manager_find_first_vacant(&train) == 2);

    assert(seat_manager_allocate(&train, 2) == RAILWAY_SUCCESS);
    assert(seat_manager_allocate(&train, 3) == RAILWAY_SUCCESS);
    assert(seat_manager_find_first_vacant(&train) == 0); /* Full */

    /* Duplicate allocation */
    assert(seat_manager_allocate(&train, 2) == RAILWAY_ERR_CAPACITY_FULL);

    /* Release and reuse */
    assert(seat_manager_release(&train, 2) == RAILWAY_SUCCESS);
    assert(seat_manager_find_first_vacant(&train) == 2);
    assert(seat_manager_allocate(&train, 2) == RAILWAY_SUCCESS);

    train_cleanup(&train);
}

/* 4. Complete Booking, Cancellation, and Promotion End-to-End Suite */
static void test_booking_and_cancellation_e2e_suite(void)
{
    RailwaySystem sys;
    assert(railway_init(&sys) == RAILWAY_SUCCESS);
    assert(railway_init_sample_trains(&sys) == RAILWAY_SUCCESS);
    assert(railway_validate_invariants(&sys) == RAILWAY_SUCCESS);

    /* Train 101 has capacity 3 */
    Booking b1, b2, b3, b_wait1, b_wait2;
    assert(railway_book_ticket(&sys, "Aarav Sharma", 28, 101, "2026-11-10", &b1) == RAILWAY_SUCCESS);
    assert(b1.seat_number == 1 && b1.status == BOOKING_CONFIRMED);

    assert(railway_book_ticket(&sys, "Bhavna Patel", 34, 101, "2026-11-10", &b2) == RAILWAY_SUCCESS);
    assert(b2.seat_number == 2 && b2.status == BOOKING_CONFIRMED);

    assert(railway_book_ticket(&sys, "Chetan Verma", 45, 101, "2026-11-10", &b3) == RAILWAY_SUCCESS);
    assert(b3.seat_number == 3 && b3.status == BOOKING_CONFIRMED);

    /* Waiting queue entries */
    assert(railway_book_ticket(&sys, "Deepika Padukone", 38, 101, "2026-11-10", &b_wait1) == RAILWAY_SUCCESS);
    assert(b_wait1.seat_number == 0 && b_wait1.status == BOOKING_WAITING);

    assert(railway_book_ticket(&sys, "Eshan Roy", 21, 101, "2026-11-10", &b_wait2) == RAILWAY_SUCCESS);
    assert(b_wait2.seat_number == 0 && b_wait2.status == BOOKING_WAITING);

    assert(railway_validate_invariants(&sys) == RAILWAY_SUCCESS);

    /* Cancel Bhavna (seat 2); Deepika should be promoted into seat 2 */
    Booking cancelled, promoted;
    assert(railway_cancel_ticket(&sys, b2.booking_id, &cancelled, &promoted) == RAILWAY_SUCCESS);
    assert(cancelled.booking_id == b2.booking_id && cancelled.status == BOOKING_CANCELLED);
    assert(promoted.booking_id == b_wait1.booking_id && promoted.status == BOOKING_CONFIRMED && promoted.seat_number == 2);

    /* Eshan should now be at front of waiting list */
    Train *t101 = railway_find_train(&sys, 101);
    assert(t101->waiting.size == 1);
    unsigned long peek_id = 0;
    assert(waiting_queue_peek(&t101->waiting, &peek_id) == RAILWAY_SUCCESS && peek_id == b_wait2.booking_id);

    assert(railway_validate_invariants(&sys) == RAILWAY_SUCCESS);

    /* Cancel Aarav (seat 1); Eshan promoted into seat 1 */
    assert(railway_cancel_ticket(&sys, b1.booking_id, &cancelled, &promoted) == RAILWAY_SUCCESS);
    assert(promoted.booking_id == b_wait2.booking_id && promoted.status == BOOKING_CONFIRMED && promoted.seat_number == 1);
    assert(t101->waiting.size == 0);

    assert(railway_validate_invariants(&sys) == RAILWAY_SUCCESS);

    railway_cleanup(&sys);
}

int main(void)
{
    printf("============================================================\n");
    printf("  BERNICE'S C CORE TEST SUITE — RAILWAY RESERVATION SYSTEM  \n");
    printf("============================================================\n");

    RUN_TEST(test_waiting_queue_suite);
    RUN_TEST(test_confirmed_list_suite);
    RUN_TEST(test_seat_manager_suite);
    RUN_TEST(test_booking_and_cancellation_e2e_suite);

    printf("============================================================\n");
    printf("  ALL TESTS PASSED: %d / %d suites passed (100%%)\n", g_tests_passed, g_tests_run);
    printf("============================================================\n");

    return 0;
}
