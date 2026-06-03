#ifndef VIDEO_H
#define VIDEO_H

#include <lcom/lcf.h>
#include <stdint.h>

/* Initialize graphics mode and map VRAM into process address space.
 * Stores mode info in static state; must be called before drawing. */
int video_init(uint16_t mode);

/* Write one pixel at (x, y) with the given color.
 * Color encoding must match the active VBE mode (e.g. 0xRRGGBB for 24-bit). */
int video_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/* Draw a filled rectangle with top-left corner at (x, y). */
int video_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/* Draw an XPM image with top-left corner at (xi, yi). */
int video_draw_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi);

/* Return horizontal resolution of the active mode. */
uint16_t video_get_hres(void);

/* Return vertical resolution of the active mode. */
uint16_t video_get_vres(void);

#endif /* VIDEO_H */
