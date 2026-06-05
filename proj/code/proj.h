#pragma once

#include "fw/hw/vbe.h"

/**
 * @file proj.h
 * @brief Project-wide configuration constants.
 */

/** @brief Timer interrupt frequency in Hz. */
#define TIMER_HZ 60

/** @brief VBE video mode used for the display (1152x864, 32-bit direct color). */
#define VIDEO_MODE VBE_864p_DC
