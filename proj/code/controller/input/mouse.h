#pragma once

#include <lcom/lcf.h>

#include "fw/drivers/mouse.h"

/**
 * @file mouse.h
 * @brief Mouse packet processing, decoded mouse event type, and cursor position accessors
 */

/** @brief Lines scrolled per scroll wheel tick */
#define SCROLL_SPEED_MULTIPLIER 3

/** @brief Decoded mouse event with click state, position, and scroll delta */
typedef struct {
  bool left_clicked;
  bool left_holding;
  int click_x, click_y;
  int scroll;
} MouseEvent;

/**
 * @brief Processes a raw mouse packet into a MouseEvent and pushes it to the input queue
 * @param pp Mouse packet from the mouse interrupt handler
 */
void mouse_process(mouse_packet pp);

/**
 * @brief Returns the current mouse cursor x position in pixels
 * @return Pixel x coordinate
 */
int ih_get_mouse_x();

/**
 * @brief Returns the current mouse cursor y position in pixels
 * @return Pixel y coordinate
 */
int ih_get_mouse_y();
