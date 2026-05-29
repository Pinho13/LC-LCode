#include "filetree_commands.h"
#include "commands.h"
#include "model/filetree.h"
#include "model/command_bar.h"
#include "view/scene.h"
#include "render_flag.h"
#include <string.h>

static void show_filetree_error() {
  const char *err = filetree_get_error();
  if (err) command_bar_set_status(err);
}

static void dispatch_filetree_mode(KeyEvent ev) {
  if (ev.escape) {
    filetree_set_focused(false);
    set_render(RENDER_FULL);
    return;
  }
  if (ev.dir == DIR_UP) {
    filetree_move_up();
    set_render(RENDER_FULL);
    return;
  }
  if (ev.dir == DIR_DOWN) {
    filetree_move_down(scene_get_vis_rows());
    set_render(RENDER_FULL);
    return;
  }
  if (ev.backspace || ev.dir == DIR_LEFT) {
    if (filetree_go_parent() == FILETREE_ERR) show_filetree_error();
    set_render(RENDER_FULL);
    return;
  }
  if (ev.enter) {
    FiletreeResult result = filetree_enter_selected();
    if (result == FILETREE_FILE) {
      char path[512];
      filetree_get_selected_path(path, sizeof(path));
      commands_open_file(path);
      filetree_set_focused(false);
    } else if (result == FILETREE_ERR) {
      show_filetree_error();
    }
    set_render(RENDER_FULL);
  }
}

bool filetree_commands_try(KeyEvent ev) {
  if (filetree_is_focused()) {
    dispatch_filetree_mode(ev);
    return true;
  }
  if (ev.ctrl && ev.c == 'b') {
    bool now_visible = !filetree_is_visible();
    filetree_set_visible(now_visible);
    filetree_set_focused(now_visible);
    set_render(RENDER_FULL);
    return true;
  }
  return false;
}
