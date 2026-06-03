#pragma once

#include <lcom/lcf.h>

#include "fw/drivers/mouse.h"

typedef struct {
  bool left_clicked;
  int click_x, click_y;
} MouseEvent;


void mouse_process(struct packet pp);

int ih_get_mouse_x();
int ih_get_mouse_y();
