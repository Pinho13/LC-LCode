#pragma once

#include <lcom/lcf.h>


#define RENDER_NONE   0
#define RENDER_CURSOR 1
#define RENDER_CHAR   2
#define RENDER_WORD   3
#define RENDER_FULL   4

void set_render(int mode);
int get_render();
void clear_render();
