#include "video.h"

static void *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;
static unsigned frame_size;

int set_graphics_mode(uint16_t mode) {
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  r86.intno = 0x10;
  r86.ah = 0x4F;
  r86.al = 0x02;
  r86.bx = mode | BIT(14);

  if (sys_int86(&r86) != OK) return 1;

  return 0;
}

// Function wrapper only for consistency reasons
int set_text_mode() {
  return vg_exit();
}


int vg_map_vram(uint16_t mode) {
  vbe_mode_info_t info;
  memset(&info, 0, sizeof(info));

  if (vbe_get_mode_info(mode, &info) != OK) return 1;

  bytes_per_pixel = (info.BitsPerPixel + 7)/8;
  h_res = info.XResolution;
  v_res = info.YResolution;
  frame_size = h_res * v_res * bytes_per_pixel;

  struct minix_mem_range physic_address;
  physic_address.mr_base = info.PhysBasePtr;
  physic_address.mr_limit = info.PhysBasePtr + frame_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &physic_address) != OK)
    return 1;

  video_mem = vm_map_phys(SELF, (void*) physic_address.mr_base, frame_size);
  if (video_mem == MAP_FAILED) return 1;

  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if(x >= h_res || y >= v_res) return 1;

  unsigned index = (h_res * y + x) * bytes_per_pixel;

  memcpy((uint8_t *) video_mem + index, &color, bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (unsigned i = 0; i < len; i++)
    if (vg_draw_pixel(x+i, y, color) != OK) return 1;

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for(unsigned i = 0; i < height ; i++)
    if (vg_draw_hline(x, y+i, width, color) != OK) return 1;

  return 0;
}
