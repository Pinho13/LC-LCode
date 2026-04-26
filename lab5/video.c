#include "video.h"

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
