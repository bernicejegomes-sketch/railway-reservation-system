#include <stdio.h>
#include <assert.h>
#include "waiting_queue.h"
#include "railway.h"

int main(void)
{
    printf("Running Waiting Queue Tests...\n");

    WaitingQueue queue;
    int res = waiting_queue_init(&queue);
    assert(res == RAILWAY_SUCCESS);

    /* Test 1: Empty queue properties */
    assert(waiting_queue_is_empty(&queue) == 1);
    assert(queue.front == NULL);
    assert(queue.rear == NULL);
    assert(queue.size == 0);

    unsigned long peek_id = 0;
    assert(waiting_queue_peek(&queue, &peek_id) == RAILWAY_ERR_INVALID_PARAM);

    unsigned long deq_id = 0;
    assert(waiting_queue_dequeue(&queue, &deq_id) == RAILWAY_ERR_INVALID_PARAM);

    /* Test 2: Single insertion */
    res = waiting_queue_enqueue(&queue, 1001);
    assert(res == RAILWAY_SUCCESS);
    assert(waiting_queue_is_empty(&queue) == 0);
    assert(queue.size == 1);
    assert(queue.front != NULL);
    assert(queue.rear != NULL);
    assert(queue.front == queue.rear); /* Both front and rear point to the same single node */
    assert(queue.front->booking_id == 1001);
    assert(queue.front->next == NULL);

    /* Test 3: Peek operation */
    peek_id = 0;
    res = waiting_queue_peek(&queue, &peek_id);
    assert(res == RAILWAY_SUCCESS);
    assert(peek_id == 1001);
    assert(queue.size == 1); /* Peek must not remove */

    /* Test 4: Position query */
    size_t pos = 0;
    res = waiting_queue_get_position(&queue, 1001, &pos);
    assert(res == RAILWAY_SUCCESS);
    assert(pos == 1);

    /* Test 5: Removing the single element (pointer reset to NULL) */
    deq_id = 0;
    res = waiting_queue_dequeue(&queue, &deq_id);
    assert(res == RAILWAY_SUCCESS);
    assert(deq_id == 1001);
    assert(waiting_queue_is_empty(&queue) == 1);
    assert(queue.front == NULL);
    assert(queue.rear == NULL); /* Crucial: rear must be reset to NULL */
    assert(queue.size == 0);

    /* Test 6: Multiple insertions and strict FIFO ordering */
    assert(waiting_queue_enqueue(&queue, 2001) == RAILWAY_SUCCESS);
    assert(waiting_queue_enqueue(&queue, 2002) == RAILWAY_SUCCESS);
    assert(waiting_queue_enqueue(&queue, 2003) == RAILWAY_SUCCESS);
    assert(queue.size == 3);
    assert(queue.front->booking_id == 2001);
    assert(queue.rear->booking_id == 2003);
    assert(queue.rear->next == NULL);

    /* Check positions in queue */
    assert(waiting_queue_get_position(&queue, 2001, &pos) == RAILWAY_SUCCESS && pos == 1);
    assert(waiting_queue_get_position(&queue, 2002, &pos) == RAILWAY_SUCCESS && pos == 2);
    assert(waiting_queue_get_position(&queue, 2003, &pos) == RAILWAY_SUCCESS && pos == 3);
    assert(waiting_queue_get_position(&queue, 9999, &pos) == RAILWAY_ERR_BOOKING_NOT_FOUND);

    /* Verify FIFO removal order */
    assert(waiting_queue_dequeue(&queue, &deq_id) == RAILWAY_SUCCESS && deq_id == 2001);
    assert(queue.size == 2);
    assert(queue.front->booking_id == 2002);

    assert(waiting_queue_dequeue(&queue, &deq_id) == RAILWAY_SUCCESS && deq_id == 2002);
    assert(queue.size == 1);
    assert(queue.front == queue.rear);
    assert(queue.front->booking_id == 2003);

    assert(waiting_queue_dequeue(&queue, &deq_id) == RAILWAY_SUCCESS && deq_id == 2003);
    assert(queue.size == 0);
    assert(queue.front == NULL);
    assert(queue.rear == NULL);

    /* Test 7: Queue clear operation */
    assert(waiting_queue_enqueue(&queue, 3001) == RAILWAY_SUCCESS);
    assert(waiting_queue_enqueue(&queue, 3002) == RAILWAY_SUCCESS);
    waiting_queue_clear(&queue);
    assert(queue.front == NULL);
    assert(queue.rear == NULL);
    assert(queue.size == 0);

    printf("Waiting Queue Tests PASSED.\n");
    return 0;
}
