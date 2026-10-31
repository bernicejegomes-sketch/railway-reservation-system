#include <stdio.h>
#include <assert.h>
#include "confirmed_list.h"
#include "railway.h"

int main(void)
{
    printf("Running Confirmed List Tests...\n");

    ConfirmedList list;
    int res = confirmed_list_init(&list);
    assert(res == RAILWAY_SUCCESS);
    assert(list.head == NULL);
    assert(list.size == 0);

    /* Test 1: Insertion and size check */
    assert(confirmed_list_insert(&list, 101) == RAILWAY_SUCCESS);
    assert(list.size == 1);
    assert(list.head != NULL);
    assert(list.head->booking_id == 101);
    assert(confirmed_list_contains(&list, 101) == 1);
    assert(confirmed_list_contains(&list, 999) == 0);

    /* Test 2: Multiple insertions (inserts at head) */
    assert(confirmed_list_insert(&list, 102) == RAILWAY_SUCCESS);
    assert(confirmed_list_insert(&list, 103) == RAILWAY_SUCCESS);
    assert(list.size == 3);
    assert(list.head->booking_id == 103);
    assert(list.head->next->booking_id == 102);
    assert(list.head->next->next->booking_id == 101);

    /* Test 3: Removal from head */
    assert(confirmed_list_remove(&list, 103) == RAILWAY_SUCCESS);
    assert(list.size == 2);
    assert(list.head->booking_id == 102);
    assert(confirmed_list_contains(&list, 103) == 0);

    /* Test 4: Removal from middle */
    assert(confirmed_list_insert(&list, 104) == RAILWAY_SUCCESS); /* List is now 104 -> 102 -> 101 */
    assert(confirmed_list_remove(&list, 102) == RAILWAY_SUCCESS); /* Remove middle */
    assert(list.size == 2);
    assert(list.head->booking_id == 104);
    assert(list.head->next->booking_id == 101);
    assert(confirmed_list_contains(&list, 102) == 0);

    /* Test 5: Removal from tail */
    assert(confirmed_list_remove(&list, 101) == RAILWAY_SUCCESS);
    assert(list.size == 1);
    assert(list.head->booking_id == 104);
    assert(list.head->next == NULL);

    /* Test 6: Removing non-existent booking */
    assert(confirmed_list_remove(&list, 888) == RAILWAY_ERR_BOOKING_NOT_FOUND);
    assert(list.size == 1);

    /* Test 7: Clear list */
    confirmed_list_clear(&list);
    assert(list.head == NULL);
    assert(list.size == 0);
    assert(confirmed_list_contains(&list, 104) == 0);

    printf("Confirmed List Tests PASSED.\n");
    return 0;
}
