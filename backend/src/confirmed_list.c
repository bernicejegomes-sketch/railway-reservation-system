#include <stdlib.h>
#include "confirmed_list.h"
#include "railway.h"

int confirmed_list_init(ConfirmedList *list)
{
    if (list == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    list->head = NULL;
    list->size = 0;
    return RAILWAY_SUCCESS;
}

int confirmed_list_insert(ConfirmedList *list, unsigned long booking_id)
{
    if (list == NULL || booking_id == 0) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    ConfirmedNode *node = (ConfirmedNode *)malloc(sizeof(ConfirmedNode));
    if (node == NULL) {
        return RAILWAY_ERR_ALLOCATION_FAILED;
    }

    node->booking_id = booking_id;
    node->next = list->head;
    list->head = node;
    list->size++;

    return RAILWAY_SUCCESS;
}

int confirmed_list_remove(ConfirmedList *list, unsigned long booking_id)
{
    if (list == NULL || list->head == NULL || booking_id == 0) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    ConfirmedNode *current = list->head;
    ConfirmedNode *previous = NULL;

    while (current != NULL) {
        if (current->booking_id == booking_id) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            list->size--;
            return RAILWAY_SUCCESS;
        }

        previous = current;
        current = current->next;
    }

    return RAILWAY_ERR_BOOKING_NOT_FOUND;
}

int confirmed_list_contains(const ConfirmedList *list, unsigned long booking_id)
{
    if (list == NULL || booking_id == 0) {
        return 0;
    }

    const ConfirmedNode *current = list->head;
    while (current != NULL) {
        if (current->booking_id == booking_id) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void confirmed_list_clear(ConfirmedList *list)
{
    if (list == NULL) {
        return;
    }

    ConfirmedNode *current = list->head;
    while (current != NULL) {
        ConfirmedNode *next_node = current->next;
        free(current);
        current = next_node;
    }

    list->head = NULL;
    list->size = 0;
}
