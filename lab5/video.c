#include <lcom/lcf.h>

#include "video.h"

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;

int vg_map_vram(uint16_t mode) {
  vbe_mode_info_t info;
  memset(&info, 0, sizeof(info));

  if (vbe_get_mode_info(mode, &info) != OK) {
    return 1;
  }

  bytes_per_pixel = (info.BitsPerPixel + 7) / 8;
  h_res = info.XResolution;
  v_res = info.YResolution;
  unsigned frame_size = h_res * v_res * bytes_per_pixel;

  struct minix_mem_range mr;
  mr.mr_base = info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + frame_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
    return 1;
  }

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, frame_size);

  if (video_mem == MAP_FAILED) {
    return 1;
  }

  return 0;
}

int vg_set_mode(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = 0x10;
  reg.ah = 0x4F;
  reg.al = VBE_SET_MODE_FUNC;
  reg.bx = mode | VBE_LINEAR_FB;

  if (sys_int86(&reg) != OK) {
    return 1;
  }

  if (reg.al != 0x4F || reg.ah != 0x00) {
    return 1;
  }

  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= h_res || y >= v_res) {
    return 0;
  }

  unsigned index = (h_res * y + x) * bytes_per_pixel;
  memcpy((uint8_t *) video_mem + index, &color, bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i = 0; i < len; i++) {
    if (vg_draw_pixel(x + i, y, color) != 0) {
      return 1;
    }
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color) != 0) {
      return 1;
    }
  }
  return 0;
}

int (print_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  xpm_image_t img;
  uint8_t *colors = xpm_load(xpm, XPM_INDEXED, &img);

  if (colors == NULL) {
    return 1;
  }

  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      if (vg_draw_pixel(x + col, y + row, colors[col + row * img.width]) != 0) {
        return 1;
      }
    }
  }

  return 0;
}
