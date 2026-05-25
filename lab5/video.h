#ifndef _LCOM_VIDEO_H_
#define _LCOM_VIDEO_H_

#include <lcom/lcf.h>

#define VBE_SET_MODE_FUNC 0x02
#define VBE_LINEAR_FB     BIT(14)

int vg_map_vram(uint16_t mode);
int vg_set_mode(uint16_t mode);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

#endif /* _LCOM_VIDEO_H_ */
