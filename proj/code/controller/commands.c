#include "commands.h"
#include "model/editor.h"
#include "render_flag.h"

static bool quit_flag = false;

bool get_quit() { return quit_flag; }

void commands_dispatch(KeyEvent ev) {
  if (ev.escape || (ev.ctrl && ev.c == 'q')) {
    quit_flag = true;
    return;
  }

  if (ev.backspace) {
    if (ev.ctrl) {
      editor_delete_word();
      set_render(RENDER_WORD);
    } else {
      editor_delete_char();
      set_render(RENDER_CHAR);
    }
    return;
  }

  if (ev.ctrl) return;

  if (ev.enter) {
    editor_insert_char('\n');
    set_render(RENDER_FULL);
    return;
  }

  if (ev.c) {
    editor_insert_char(ev.c);
    set_render(RENDER_CHAR);
  }
}
