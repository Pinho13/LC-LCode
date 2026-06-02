#include "fw/common/queue.h"

void queue_init(queue_t *q) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
}

bool queue_is_empty(queue_t *q) { return (q->count == 0); }

bool queue_is_full(queue_t *q) { return (q->count == QUEUE_SIZE); }

bool queue_push(queue_t *q, uint8_t data) {
  if (queue_is_full(q))
    return false;
  q->buffer[q->tail] = data;
  q->tail = (q->tail + 1) % QUEUE_SIZE;
  q->count++;
  return true;
}

bool queue_pop(queue_t *q, uint8_t *data) {
  if (queue_is_empty(q))
    return false;
  *data = q->buffer[q->head];
  q->head = (q->head + 1) % QUEUE_SIZE;
  q->count--;
  return true;
}

