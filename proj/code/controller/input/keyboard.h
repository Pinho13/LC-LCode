#pragma once

#include <lcom/lcf.h>
#include <stdbool.h>

#include "fw/drivers/keyboard.h"

/**
 * @file keyboard.h
 * @brief Keyboard scancode processing and decoded key event type
 */

/** @brief Cursor movement directions, decoded from arrow and navigation scancodes */
typedef enum {
  DIR_NONE,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN,
  DIR_HOME,
  DIR_END
} Direction;

/** @brief Decoded keyboard event with character, modifier flags, and direction */
typedef struct {
  char c;
  bool ctrl;
  bool shift;
  bool altgr;
  bool backspace;
  bool enter;
  bool escape;
  Direction dir;
} KeyEvent;

/**
 * @brief Processes a raw scancode packet into a KeyEvent and pushes it to the input queue
 * @param ps Scancode packet from the keyboard interrupt handler
 */
void keyboard_process(packet_scancode ps);
