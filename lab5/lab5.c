#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>

#include "video.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
  lcf_log_output("/home/lcom/labs/lab5/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

/* ---------------------------------------------------------------------------
 * video_test_init -- switch to graphics mode, wait, return to text.
 *
 * Steps (from the guide):
 *   1. Switch to the graphics mode given by `mode`
 *   2. Wait `delay` seconds
 *   3. Return to text mode via vg_exit() (provided by the LCF)
 *
 * Why tickdelay instead of sleep:
 *   The guide explicitly forbids sleep(). tickdelay + micros_to_ticks is the
 *   MINIX-approved way to wait; it uses the hardware timer without blocking
 *   the OS scheduler in an uncooperative way.
 * ---------------------------------------------------------------------------*/
int (video_test_init)(uint16_t mode, uint8_t delay) {
  if (video_init(mode) != 0)
    return 1;

  tickdelay(micros_to_ticks((uint32_t)delay * 1000000));

  return vg_exit();
}

/* ---------------------------------------------------------------------------
 * video_test_rect -- switch to graphics mode, draw a filled rectangle, wait,
 *                    return to text.
 *
 * The color encoding must match the active VBE mode.  The LCF passes a color
 * that is already encoded correctly for the mode under test, so we pass it
 * through to video_draw_rect unchanged.
 * ---------------------------------------------------------------------------*/
int (video_test_rect)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width,
                      uint16_t height, uint32_t color) {
  if (video_init(mode) != 0)
    return 1;

  if (video_draw_rect(x, y, width, height, color) != 0) {
    vg_exit();
    return 1;
  }

  tickdelay(micros_to_ticks(3000000));  /* 3-second display window */

  return vg_exit();
}

/* ---------------------------------------------------------------------------
 * video_test_xpm -- switch to graphics mode, render an XPM at (xi, yi),
 *                   wait, return to text.
 * ---------------------------------------------------------------------------*/
int (video_test_xpm)(xpm_map_t xpm, uint16_t xi, uint16_t yi) {
  /* The guide uses mode 0x105 (1024x768, indexed 8-bit) for XPM tests */
  if (video_init(0x105) != 0)
    return 1;

  if (video_draw_xpm(xpm, xi, yi) != 0) {
    vg_exit();
    return 1;
  }

  tickdelay(micros_to_ticks(3000000));

  return vg_exit();
}
