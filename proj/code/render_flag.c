#include "render_flag.h"

static int render_mode = RENDER_NONE;

void set_render(int mode) {
  if (mode > render_mode) render_mode = mode;
}

int get_render() { return render_mode; }
void clear_render() { render_mode = RENDER_NONE; }
