#include <lcom/lcf.h>
#include <stdint.h>
#include <string.h>

#include "video.h"

/* ---- Static state for the active VBE mode ---- */
static char           *video_mem;        /* virtual address of frame buffer  */
static vbe_mode_info_t vmi;              /* VBE mode info from vbe_get_mode_info */
static unsigned        bytes_per_pixel;  /* derived from vmi.BitsPerPixel     */

uint16_t video_get_hres(void) { return vmi.XResolution; }
uint16_t video_get_vres(void) { return vmi.YResolution; }

/*
 * Switch to the requested VBE graphics mode and map VRAM.
 *
 * Steps (from the guide):
 *   1. Call vbe_get_mode_info() to fill vmi
 *   2. Invoke VBE Set Mode (AH=0x4F, AL=0x02) via sys_int86
 *      BX = mode | BIT(14)  -- bit 14: use linear frame buffer
 *      BX must NOT have bit 15 set -- that would preserve VRAM contents
 *      (we want VRAM cleared, so bit 15 stays 0)
 *   3. Grant memory mapping permission with sys_privctl / SYS_PRIV_ADD_MEM
 *   4. Map with vm_map_phys; store result in video_mem
 */
int video_init(uint16_t mode) {
  if (vbe_get_mode_info(mode, &vmi) != 0)
    return 1;

  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;

  /* Set VBE mode */
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.intno = 0x10;
  reg.ah    = 0x4F;
  reg.al    = 0x02;
  reg.bx    = mode | BIT(14);  /* linear frame buffer; bit 15 clear = clear VRAM */

  if (sys_int86(&reg) != OK)
    return 1;

  /* Size of frame buffer in bytes */
  unsigned vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

  /* Grant permission to map this physical memory range */
  struct minix_mem_range mr;
  mr.mr_base  = (phys_bytes) vmi.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
    return 1;

  /* Map into our virtual address space */
  video_mem = vm_map_phys(SELF, (void *)(phys_bytes) vmi.PhysBasePtr, vram_size);
  if (video_mem == MAP_FAILED)
    return 1;

  return 0;
}

/*
 * Write one pixel.
 *
 * Pixel offset in the linear frame buffer:
 *   offset = (y * hres + x) * bytes_per_pixel
 *
 * We use memcpy to write bytes_per_pixel bytes from color.
 * This works for 8, 15, 16, 24, and 32 bpp without branching on depth.
 * color must already be encoded in the format expected by the active mode.
 */
int video_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= vmi.XResolution || y >= vmi.YResolution)
    return 1;

  unsigned offset = (y * vmi.XResolution + x) * bytes_per_pixel;
  memcpy(video_mem + offset, &color, bytes_per_pixel);
  return 0;
}

/*
 * Draw a filled rectangle.
 * Clips silently at screen boundaries (the LCF may pass coords that go off screen).
 */
int video_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t row = y; row < y + height; row++) {
    for (uint16_t col = x; col < x + width; col++) {
      if (video_draw_pixel(col, row, color) != 0)
        return 1;
    }
  }
  return 0;
}

/*
 * Draw an XPM image at (xi, yi).
 *
 * xpm_load() (provided by the LCF) parses the XPM map and returns a flat
 * array of uint32_t ARGB pixels together with the image dimensions.
 * We iterate over each pixel and write it to the frame buffer.
 *
 * XPM_INDEXED is used for mode 0x105 (indexed 8-bit palette);
 * XPM_8_8_8_8 covers 32-bpp direct color modes.
 * The guide says to use XPM_8_8_8_8 for direct color modes.
 */
int video_draw_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi) {
  xpm_image_t img;
  uint8_t *colors = xpm_load(xpm, XPM_8_8_8_8, &img);
  if (colors == NULL)
    return 1;

  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      /* Each pixel is 4 bytes (ARGB) from xpm_load with XPM_8_8_8_8 */
      uint32_t color;
      memcpy(&color, colors + (row * img.width + col) * 4, 4);
      /* Write only bytes_per_pixel bytes to VRAM */
      uint16_t dx = xi + col;
      uint16_t dy = yi + row;
      if (dx < vmi.XResolution && dy < vmi.YResolution) {
        unsigned offset = (dy * vmi.XResolution + dx) * bytes_per_pixel;
        memcpy(video_mem + offset, &color, bytes_per_pixel);
      }
    }
  }
  return 0;
}
