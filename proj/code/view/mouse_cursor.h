#pragma once

#include <lcom/lcf.h>

int mouse_cursor_init();
void mouse_cursor_cleanup();

void mouse_cursor_hide();
void mouse_cursor_show(int x, int y);

int mouse_cursor_prev_x();
int mouse_cursor_prev_y();
int mouse_cursor_width();
int mouse_cursor_height();
