#ifndef CONFIRMED_LIST_H
#define CONFIRMED_LIST_H

#include <stddef.h>

/* Forward declaration and definition of singly linked list node */
typedef struct ConfirmedNode ConfirmedNode;

struct ConfirmedNode {
    unsigned long booking_id;  /* References central booking registry */
    ConfirmedNode *next;       /* Pointer to next node */
};

/* Singly linked list for confirmed bookings */
typedef struct {
    ConfirmedNode *head;       /* Pointer to head node */
    size_t size;               /* Total nodes in list */
} ConfirmedList;

/* Operations */
int confirmed_list_init(ConfirmedList *list);
int confirmed_list_insert(ConfirmedList *list, unsigned long booking_id);
int confirmed_list_remove(ConfirmedList *list, unsigned long booking_id);
int confirmed_list_contains(const ConfirmedList *list, unsigned long booking_id);
void confirmed_list_clear(ConfirmedList *list);

#endif /* CONFIRMED_LIST_H */
