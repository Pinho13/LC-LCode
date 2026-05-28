#pragma once

#include <lcom/lcf.h>


#define RENDER_NONE   0
#define RENDER_CURSOR 1
#define RENDER_CHAR   2
#define RENDER_FULL   3

void set_render(int mode);
int get_render();
void clear_render();
