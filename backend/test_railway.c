#include "railway.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures;

#define CHECK(condition)                                                     \
    do {                                                                     \
        if (!(condition)) {                                                  \
            printf("FAIL line %d: %s\n", __LINE__, #condition);              \
            failures++;                                                      \
        }                                                                    \
    } while (0)

static Passenger *book(int train) {
    Passenger *p = NULL;
    CHECK(rail_book(train, "Test Passenger", 30, 'F', &p) == NULL);
    return p;
}

static void fill_seats(int train) {
    for (int i = 0; i < SEATS_PER_TRAIN; i++)
        book(train);
}

static void test_booking_order(void) {
    rail_init();
    Passenger *first = book(0), *second = book(0);
    CHECK(first->status == CONFIRMED && first->seat == 1);
    CHECK(second->seat == 2);
    CHECK(second->pnr != first->pnr);
    CHECK(trains[0].free_top == SEATS_PER_TRAIN - 2);
    CHECK(trains[1].free_top == SEATS_PER_TRAIN); /* other trains are unaffected */
}

static void test_validation(void) {
    rail_init();
    Passenger *p;
    CHECK(rail_book(0, "", 30, 'M', &p) != NULL);
    CHECK(rail_book(0, " Padded", 30, 'M', &p) != NULL);
    CHECK(rail_book(0, "Quote\"Name", 30, 'M', &p) != NULL);
    CHECK(rail_book(0, "Back\\slash", 30, 'M', &p) != NULL);
    CHECK(rail_book(0, "Valid Name", 0, 'M', &p) != NULL);
    CHECK(rail_book(0, "Valid Name", 121, 'M', &p) != NULL);
    CHECK(rail_book(0, "Valid Name", 30, 'X', &p) != NULL);
    CHECK(rail_book(-1, "Valid Name", 30, 'M', &p) != NULL);
    CHECK(rail_book(TRAIN_COUNT, "Valid Name", 30, 'M', &p) != NULL);
    CHECK(rail_book(0, "Mary-Ann O'Neil Jr.", 30, 'F', &p) == NULL);
}

static void test_waiting_list_and_limit(void) {
    rail_init();
    fill_seats(0);
    for (int i = 1; i <= WAITLIST_LIMIT; i++) {
        Passenger *p = book(0);
        CHECK(p->status == WAITING && p->seat == 0);
        CHECK(rail_wait_position(p) == i);
    }
    Passenger *p = NULL;
    CHECK(rail_book(0, "One Too Many", 30, 'M', &p) != NULL);
    CHECK(trains[0].wait_count == WAITLIST_LIMIT);
}

static void test_cancel_confirmed_promotes_first_in_line(void) {
    rail_init();
    Passenger *seat5 = NULL;
    for (int i = 1; i <= SEATS_PER_TRAIN; i++) {
        Passenger *p = book(0);
        if (i == 5)
            seat5 = p;
    }
    Passenger *wait1 = book(0), *wait2 = book(0);
    int cancelled = seat5->pnr;

    Passenger *promoted;
    CHECK(rail_cancel(cancelled, &promoted) == 0);
    CHECK(promoted == wait1);
    CHECK(wait1->status == CONFIRMED && wait1->seat == 5);
    CHECK(trains[0].seats[4] == wait1);
    CHECK(rail_wait_position(wait2) == 1);
    CHECK(trains[0].wait_count == 1);
    CHECK(trains[0].free_top == 0);
    CHECK(rail_find(cancelled) == NULL);
}

static void test_cancel_waiting_keeps_queue_order(void) {
    rail_init();
    fill_seats(0);
    Passenger *a = book(0), *b = book(0), *c = book(0);

    Passenger *promoted;
    CHECK(rail_cancel(b->pnr, &promoted) == 0); /* middle */
    CHECK(promoted == NULL);
    CHECK(a->next == c && c->prev == a);
    CHECK(rail_wait_position(c) == 2);

    CHECK(rail_cancel(c->pnr, &promoted) == 0); /* tail */
    CHECK(trains[0].wait_tail == a);

    CHECK(rail_cancel(a->pnr, &promoted) == 0); /* last one */
    CHECK(trains[0].wait_head == NULL && trains[0].wait_tail == NULL);
    CHECK(trains[0].wait_count == 0);
}

static void test_freed_seat_is_reused(void) {
    rail_init();
    book(0);
    Passenger *second = book(0);
    book(0);

    Passenger *promoted;
    CHECK(rail_cancel(second->pnr, &promoted) == 0);
    CHECK(promoted == NULL);
    CHECK(book(0)->seat == 2);
}

static void test_unknown_pnr(void) {
    rail_init();
    Passenger *promoted;
    CHECK(rail_find(999) == NULL);
    CHECK(rail_find(-1) == NULL);
    CHECK(rail_cancel(999, &promoted) == -1);
}

static void test_many_passengers_in_hash_table(void) {
    rail_init();
    int pnrs[TRAIN_COUNT * (SEATS_PER_TRAIN + WAITLIST_LIMIT)];
    int count = 0;
    for (int t = 0; t < TRAIN_COUNT; t++)
        for (int i = 0; i < SEATS_PER_TRAIN + WAITLIST_LIMIT; i++)
            pnrs[count++] = book(t)->pnr;
    for (int i = 0; i < count; i++) {
        Passenger *p = rail_find(pnrs[i]);
        CHECK(p && p->pnr == pnrs[i]);
    }
}

static void test_save_and_load(void) {
    const char *path = "test_records.csv";
    rail_init();
    fill_seats(0);
    Passenger *wait1 = book(0), *wait2 = book(0);
    int wait1_pnr = wait1->pnr, wait2_pnr = wait2->pnr;
    Passenger *on_seat3 = trains[0].seats[2];
    int seat3_pnr = on_seat3->pnr;
    Passenger *other = book(1), *promoted;
    int other_pnr = other->pnr;
    CHECK(rail_cancel(trains[0].seats[0]->pnr, &promoted) == 0); /* wait1 takes seat 1 */
    CHECK(rail_save(path) == 0);

    rail_init();
    CHECK(rail_load(path) == 0);
    CHECK(rail_find(seat3_pnr)->seat == 3);
    CHECK(rail_find(wait1_pnr)->seat == 1);
    CHECK(rail_find(wait2_pnr)->status == WAITING);
    CHECK(trains[0].wait_count == 1 && trains[0].free_top == 0);
    CHECK(rail_find(other_pnr)->seat == 1);
    CHECK(trains[1].free_top == SEATS_PER_TRAIN - 1);
    CHECK(strcmp(rail_find(other_pnr)->name, "Test Passenger") == 0);

    /* PNRs keep counting from where they stopped, and the free stack works. */
    Passenger *fresh = book(1);
    CHECK(fresh->pnr > other_pnr && fresh->seat == 2);

    /* A damaged file is refused rather than half loaded. */
    FILE *f = fopen(path, "a");
    fputs("garbage line\n", f);
    fclose(f);
    CHECK(rail_load(path) == -1);
    CHECK(rail_find(seat3_pnr) == NULL);

    remove(path);
    CHECK(rail_load(path) == 0); /* a missing file is a first run */
}

int main(void) {
    test_booking_order();
    test_validation();
    test_waiting_list_and_limit();
    test_cancel_confirmed_promotes_first_in_line();
    test_cancel_waiting_keeps_queue_order();
    test_freed_seat_is_reused();
    test_unknown_pnr();
    test_many_passengers_in_hash_table();
    test_save_and_load();

    if (failures) {
        printf("%d check(s) failed\n", failures);
        return EXIT_FAILURE;
    }
    printf("All tests passed\n");
    return EXIT_SUCCESS;
}
