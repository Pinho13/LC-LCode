#pragma once

#include <lcom/lcf.h>

#define QUEUE_SIZE 16

typedef struct {
    uint8_t buffer[QUEUE_SIZE];
    int head;
    int tail;
    int count;
} queue_t;

void queue_init(queue_t *q);
bool queue_is_empty(queue_t *q);
bool queue_is_full(queue_t *q);
bool queue_push(queue_t *q, uint8_t data);
bool queue_pop(queue_t *q, uint8_t *data);
