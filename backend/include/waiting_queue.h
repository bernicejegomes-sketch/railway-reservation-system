#ifndef WAITING_QUEUE_H
#define WAITING_QUEUE_H

#include <stddef.h>

/* Forward declaration and definition of waiting queue node */
typedef struct WaitingNode WaitingNode;

struct WaitingNode {
    unsigned long booking_id;  /* References central booking registry */
    WaitingNode *next;         /* Pointer to next waiting node */
};

/* FIFO Linked Queue with front and rear pointers */
typedef struct {
    WaitingNode *front;        /* Head of queue: earliest passenger */
    WaitingNode *rear;         /* Tail of queue: latest passenger */
    size_t size;               /* Total passengers waiting */
} WaitingQueue;

/* Operations */
int waiting_queue_init(WaitingQueue *queue);
int waiting_queue_enqueue(WaitingQueue *queue, unsigned long booking_id);
int waiting_queue_dequeue(WaitingQueue *queue, unsigned long *booking_id_out);
int waiting_queue_peek(const WaitingQueue *queue, unsigned long *booking_id_out);
int waiting_queue_is_empty(const WaitingQueue *queue);
int waiting_queue_get_position(const WaitingQueue *queue, unsigned long booking_id, size_t *position_out);
int waiting_queue_contains(const WaitingQueue *queue, unsigned long booking_id);
void waiting_queue_clear(WaitingQueue *queue);

#endif /* WAITING_QUEUE_H */
