#include "scene.h"
#include "video.h"
#include "font.h"
#include "render_flag.h"
#include "model/editor.h"

#define COLOR_BG 0x1E1E1E
#define COLOR_TEXT 0xFFFFFF

#define EDITOR_X 10
#define EDITOR_Y 10

static SceneID current_scene = SCENE_EDITOR;
static int prev_col = 0;
static int prev_row = 0;

static void draw_cell(int col, int row) {
  int x = EDITOR_X + col * FONT_W;
  int y = EDITOR_Y + row * FONT_H;
  bb_draw_rect(x, y, FONT_W, FONT_H, COLOR_BG);
  const char *line = editor_get_line(row);
  if (line[col]) draw_char(x, y, line[col], COLOR_TEXT);
}

static void draw_cursor(int col, int row) {
  int x = EDITOR_X + col * FONT_W;
  int y = EDITOR_Y + row * FONT_H;
  bb_draw_rect(x, y, FONT_W, FONT_H, COLOR_TEXT);
}

static void flip_cell(int col, int row) {
  vg_flip_region(EDITOR_X + col * FONT_W, EDITOR_Y + row * FONT_H, FONT_W, FONT_H);
}

int scene_init(SceneID id) {
  current_scene = id;
  set_render(RENDER_FULL);
  return 0;
}

void scene_cleanup() {}

void view_render() {
  int mode = get_render();
  clear_render();

  int col = editor_get_cursor_col();
  int row = editor_get_cursor_row();

  switch (current_scene) {
    case SCENE_EDITOR:
      switch (mode) {
        case RENDER_FULL:
          bb_clear(COLOR_BG);
          for (int r = 0; r < editor_get_row_count(); r++)
            draw_string(EDITOR_X, EDITOR_Y + r * FONT_H, editor_get_line(r), COLOR_TEXT);
          draw_cursor(col, row);
          vg_flip_buffer();
          prev_col = col;
          prev_row = row;
          break;

        case RENDER_CHAR:
          draw_cell(prev_col, prev_row);
          flip_cell(prev_col, prev_row);
          draw_cursor(col, row);
          flip_cell(col, row);
          prev_col = col;
          prev_row = row;
          break;

        case RENDER_WORD: {
          int end = prev_col;
          for (int c = col; c <= end; c++) draw_cell(c, prev_row);
          draw_cursor(col, row);
          vg_flip_region(EDITOR_X + col * FONT_W, EDITOR_Y + prev_row * FONT_H,
                         (end - col + 1) * FONT_W, FONT_H);
          prev_col = col;
          prev_row = row;
          break;
        }

        case RENDER_CURSOR:
          draw_cursor(col, row);
          flip_cell(col, row);
          break;

        default: break;
      }
      break;
  }
}
