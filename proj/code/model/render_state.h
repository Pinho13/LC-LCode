#pragma once

#include <lcom/lcf.h>

/**
 * @brief Rendering modes used to indicate the minimum region that must be redrawn.
 *
 * Higher values represent larger rendering operations. The rendering system
 * may promote a requested mode to a more expensive one when multiple updates
 * overlap (e.g., rendering both a local and a remote line results in a full
 * redraw).
 */
#define RENDER_NONE         0  /**< No rendering required. */
#define RENDER_MOUSE        1  /**< Redraw only the mouse cursor. */
#define RENDER_STATUS       2  /**< Redraw the status bar. */
#define RENDER_CHAR         3  /**< Redraw a single character. */
#define RENDER_REMOTE_LINE  4  /**< Redraw a remotely modified line. */
#define RENDER_LINE         5  /**< Redraw the current line. */
#define RENDER_WORD         6  /**< Redraw a word or text region. */
#define RENDER_FULL         7  /**< Perform a full screen redraw. */

/**
 * @brief Requests a rendering operation.
 *
 * Updates the current rendering mode if the requested mode requires a larger
 * redraw than the currently scheduled one. If both a local line redraw and a
 * remote line redraw are requested before rendering occurs, the mode is
 * promoted to ::RENDER_FULL.
 *
 * @param mode Rendering mode to request.
 */
void set_render(int mode);

/**
 * @brief Returns the currently scheduled rendering mode.
 *
 * @return The current rendering mode.
 */
int get_render();

/**
 * @brief Clears any pending rendering request.
 *
 * Resets the rendering mode to ::RENDER_NONE.
 */
void clear_render();
