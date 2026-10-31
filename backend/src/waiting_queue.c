#include <stdlib.h>
#include "waiting_queue.h"
#include "railway.h"

int waiting_queue_init(WaitingQueue *queue)
{
    if (queue == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return RAILWAY_SUCCESS;
}

int waiting_queue_is_empty(const WaitingQueue *queue)
{
    if (queue == NULL) {
        return 1;
    }
    return (queue->front == NULL || queue->size == 0);
}

int waiting_queue_enqueue(WaitingQueue *queue, unsigned long booking_id)
{
    if (queue == NULL || booking_id == 0) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    WaitingNode *node = (WaitingNode *)malloc(sizeof(WaitingNode));
    if (node == NULL) {
        return RAILWAY_ERR_ALLOCATION_FAILED;
    }

    node->booking_id = booking_id;
    node->next = NULL;

    /* If queue is empty, both front and rear point to new node */
    if (queue->rear == NULL) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }

    queue->size++;
    return RAILWAY_SUCCESS;
}

int waiting_queue_dequeue(WaitingQueue *queue, unsigned long *booking_id_out)
{
    if (queue == NULL || queue->front == NULL || booking_id_out == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    WaitingNode *node_to_remove = queue->front;
    *booking_id_out = node_to_remove->booking_id;

    queue->front = queue->front->next;

    /* If queue is now empty, reset rear to NULL as well */
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(node_to_remove);
    queue->size--;
    return RAILWAY_SUCCESS;
}

int waiting_queue_peek(const WaitingQueue *queue, unsigned long *booking_id_out)
{
    if (queue == NULL || queue->front == NULL || booking_id_out == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    *booking_id_out = queue->front->booking_id;
    return RAILWAY_SUCCESS;
}

int waiting_queue_get_position(const WaitingQueue *queue, unsigned long booking_id, size_t *position_out)
{
    if (queue == NULL || booking_id == 0 || position_out == NULL) {
        return RAILWAY_ERR_INVALID_PARAM;
    }

    const WaitingNode *current = queue->front;
    size_t position = 1;

    while (current != NULL) {
        if (current->booking_id == booking_id) {
            *position_out = position;
            return RAILWAY_SUCCESS;
        }
        current = current->next;
        position++;
    }

    return RAILWAY_ERR_BOOKING_NOT_FOUND;
}

int waiting_queue_contains(const WaitingQueue *queue, unsigned long booking_id)
{
    if (queue == NULL || booking_id == 0) {
        return 0;
    }

    const WaitingNode *current = queue->front;
    while (current != NULL) {
        if (current->booking_id == booking_id) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void waiting_queue_clear(WaitingQueue *queue)
{
    if (queue == NULL) {
        return;
    }

    WaitingNode *current = queue->front;
    while (current != NULL) {
        WaitingNode *next_node = current->next;
        free(current);
        current = next_node;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}
