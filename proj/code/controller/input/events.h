#pragma once

#include <stdbool.h>

#include "controller/input/keyboard.h"
#include "controller/input/mouse.h"
#include "controller/serial.h"

/**
 * @file events.h
 * @brief Input event queue: buffers keyboard, mouse, and serial events for deferred dispatch
 */

typedef enum {
  INPUT_EVENT_KEY,
  INPUT_EVENT_MOUSE,
  INPUT_EVENT_SERIAL
} InputEventType;

/** @brief Tagged union representing a single input event from any device */
typedef struct {
  InputEventType type;
  union {
    KeyEvent key;
    MouseEvent mouse;
    SerialEvent serial;
  } data;
} InputEvent;

/**
 * @brief Pushes an event onto the input queue
 * @param ev Event to enqueue
 * @return true on success, false if the queue is full
 */
bool input_event_push(InputEvent ev);

/**
 * @brief Pops the next event from the input queue
 * @param out Output event
 * @return true if an event was dequeued, false if the queue was empty
 */
bool input_event_pop(InputEvent *out);

/**
 * @brief Returns whether the input queue is empty
 * @return true if there are no pending events
 */
bool input_events_is_empty();

/**
 * @brief Discards all pending events in the queue
 */
void input_events_clear();

/**
 * @brief Dequeues and dispatches all pending input events via commands_dispatch
 */
void input_dispatch();
