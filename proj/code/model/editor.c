#include "editor.h"
#include <string.h>
#include <stdbool.h>

static char lines[MAX_LINES][MAX_COLS];
static int cursor_row = 0;
static int cursor_col = 0;
static int row_count = 1;

static int scroll_row = 0;
static int scroll_col = 0;
static int visible_rows = MAX_LINES;
static int visible_cols = MAX_COLS;
static bool scroll_dirty = false;

static bool sel_active = false;
static int sel_anchor_row = 0;
static int sel_anchor_col = 0;
static bool sel_dirty = false;

int editor_init() {
  memset(lines, 0, sizeof(lines));
  cursor_row = 0;
  cursor_col = 0;
  row_count = 1;
  scroll_row = 0;
  scroll_col = 0;
  scroll_dirty = false;
  sel_active = false;
  sel_dirty = false;
  return 0;
}

void editor_cleanup() {}

void editor_set_viewport(int rows, int cols) {
  visible_rows = rows;
  visible_cols = cols;
}

/* Adjusts scroll offsets to keep the cursor inside the visible viewport.
 * Called after every cursor move; sets scroll_dirty if anything changed. */
static void clamp_scroll() {
  if (cursor_row < scroll_row) {
    scroll_row = cursor_row;
    scroll_dirty = true;
  } else if (cursor_row >= scroll_row + visible_rows) {
    scroll_row = cursor_row - visible_rows + 1;
    scroll_dirty = true;
  }

  if (cursor_col < scroll_col) {
    scroll_col = cursor_col;
    scroll_dirty = true;
  } else if (cursor_col >= scroll_col + visible_cols) {
    scroll_col = cursor_col - visible_cols + 1;
    scroll_dirty = true;
  }

  if (sel_active) sel_dirty = true;
}

void editor_scroll_by(int drow, int dcol) {
  int new_row = scroll_row + drow;
  int new_col = scroll_col + dcol;

  if (new_row < 0) new_row = 0;
  if (new_row > row_count - 1) new_row = row_count - 1;
  if (new_col < 0) new_col = 0;

  if (new_row != scroll_row || new_col != scroll_col) {
    scroll_row = new_row;
    scroll_col = new_col;
    scroll_dirty = true;
  }
}

void editor_set_cursor(int row, int col) {
  if (row < 0) row = 0;
  if (row >= row_count) row = row_count - 1;
  int len = strlen(lines[row]);
  if (col < 0) col = 0;
  if (col > len) col = len;
  cursor_row = row;
  cursor_col = col;
  clamp_scroll();
}

bool editor_consume_scroll_dirty() {
  bool dirty = scroll_dirty;
  scroll_dirty = false;
  return dirty;
}

void editor_insert_char(char c) {
  if (c == '\n') {
    if (row_count >= MAX_LINES) return;
    /* Shift all rows below the current one down by one to open a slot,
     * then copy from cursor onwards into the new slot and end the
     * current line at the split point. */
    memmove(lines[cursor_row + 2], lines[cursor_row + 1], (row_count - cursor_row - 1) * MAX_COLS);
    int split = cursor_col;
    int tail = strlen(lines[cursor_row]) - split;
    memcpy(lines[cursor_row + 1], &lines[cursor_row][split], tail + 1);
    lines[cursor_row][split] = '\0';
    row_count++;
    cursor_row++;
    cursor_col = 0;
    clamp_scroll();
    return;
  }

  int len = strlen(lines[cursor_row]);
  if (len >= MAX_COLS - 1) return;
  memmove(&lines[cursor_row][cursor_col + 1], &lines[cursor_row][cursor_col], len - cursor_col + 1);
  lines[cursor_row][cursor_col] = c;
  cursor_col++;
  clamp_scroll();
}

void editor_delete_char() {
  if (cursor_col > 0) {
    cursor_col--;
    int len = strlen(lines[cursor_row]);
    memmove(&lines[cursor_row][cursor_col], &lines[cursor_row][cursor_col + 1], len - cursor_col);
  } else if (cursor_row > 0) {
    /* Merge current line into the end of the previous one, then shift
     * all rows below up by one to close the gap. */
    int prev_len = strlen(lines[cursor_row - 1]);
    int curr_len = strlen(lines[cursor_row]);
    if (prev_len + curr_len >= MAX_COLS) return;
    memcpy(&lines[cursor_row - 1][prev_len], lines[cursor_row], curr_len + 1);
    memmove(lines[cursor_row], lines[cursor_row + 1], (row_count - cursor_row - 1) * MAX_COLS);
    memset(lines[row_count - 1], 0, MAX_COLS);
    row_count--;
    cursor_row--;
    cursor_col = prev_len;
  }
  clamp_scroll();
}

void editor_delete_word() {
  if (cursor_col == 0) return;
  int end = cursor_col;
  while (cursor_col > 0 && lines[cursor_row][cursor_col - 1] == ' ') cursor_col--;
  while (cursor_col > 0 && lines[cursor_row][cursor_col - 1] != ' ') cursor_col--;
  int len = strlen(lines[cursor_row]);
  memmove(&lines[cursor_row][cursor_col], &lines[cursor_row][end], len - end + 1);
  clamp_scroll();
}

void editor_move_left() {
  if (cursor_col > 0) cursor_col--;
  else if (cursor_row > 0) { cursor_row--; cursor_col = strlen(lines[cursor_row]); }
  clamp_scroll();
}

void editor_move_right() {
  int len = strlen(lines[cursor_row]);
  if (cursor_col < len) cursor_col++;
  else if (cursor_row < row_count - 1) { cursor_row++; cursor_col = 0; }
  clamp_scroll();
}

void editor_move_up() {
  if (cursor_row > 0) {
    cursor_row--;
    int len = strlen(lines[cursor_row]);
    if (cursor_col > len) cursor_col = len;
  }
  clamp_scroll();
}

void editor_move_down() {
  if (cursor_row < row_count - 1) {
    cursor_row++;
    int len = strlen(lines[cursor_row]);
    if (cursor_col > len) cursor_col = len;
  }
  clamp_scroll();
}

void editor_move_word_left() {
  while (cursor_col > 0 && lines[cursor_row][cursor_col - 1] == ' ')
    cursor_col--;
  while (cursor_col > 0 && lines[cursor_row][cursor_col - 1] != ' ')
    cursor_col--;
  clamp_scroll();
}

void editor_move_word_right() {
  int len = strlen(lines[cursor_row]);
  while (cursor_col < len && lines[cursor_row][cursor_col] != ' ')
    cursor_col++;
  while (cursor_col < len && lines[cursor_row][cursor_col] == ' ')
    cursor_col++;
  clamp_scroll();
}

void editor_move_home() {
  cursor_col = 0;
  clamp_scroll();
}

void editor_move_end() {
  cursor_col = strlen(lines[cursor_row]);
  clamp_scroll();
}

const char *editor_get_line(int row) {
  if (row < 0 || row >= MAX_LINES) return "";
  return lines[row];
}

int editor_get_row_count() { return row_count; }
int editor_get_cursor_row() { return cursor_row; }
int editor_get_cursor_col() { return cursor_col; }
int editor_get_scroll_row() { return scroll_row; }
int editor_get_scroll_col() { return scroll_col; }

//Standardizes range so it starts before it ends
void editor_sel_get_range(int *start_row, int *start_col, int *end_row, int *end_col) {
  bool cursor_after = (cursor_row > sel_anchor_row) || (cursor_row == sel_anchor_row && cursor_col >= sel_anchor_col);
  if (cursor_after) {
    *start_row = sel_anchor_row; 
    *start_col = sel_anchor_col;
    *end_row = cursor_row; 
    *end_col = cursor_col;
  } 
  else {
    *start_row = cursor_row; 
    *start_col = cursor_col;
    *end_row = sel_anchor_row; 
    *end_col = sel_anchor_col;
  }
}

void editor_delete_selection() {
  if (!sel_active) return;
  int start_row, start_col, end_row, end_col;
  editor_sel_get_range(&start_row, &start_col, &end_row, &end_col);

  if (start_row == end_row) {
    int len = strlen(lines[start_row]);
    memmove(&lines[start_row][start_col], &lines[start_row][end_col], len - end_col + 1);
  } 
  else {
    int tail_len = strlen(lines[end_row]) - end_col;
    if (start_col + tail_len < MAX_COLS){
      memcpy(&lines[start_row][start_col], &lines[end_row][end_col], tail_len + 1);
    }
    else {
      lines[start_row][start_col] = '\0';
    }
    int removed = end_row - start_row;
    memmove(lines[start_row + 1], lines[end_row + 1], (row_count - end_row - 1) * MAX_COLS);
    memset(lines[row_count - removed], 0, removed * MAX_COLS);
    row_count -= removed;
  }

  cursor_row = start_row;
  cursor_col = start_col;
  sel_active = false;
  sel_dirty = true;
  clamp_scroll();
}

void editor_sel_set_anchor() {
  sel_anchor_row = cursor_row;
  sel_anchor_col = cursor_col;
  sel_active = true;
  sel_dirty = true;
}

void editor_sel_clear() {
  if (sel_active) sel_dirty = true;
  sel_active = false;
}

bool editor_sel_is_active() { return sel_active; }



bool editor_consume_sel_dirty() {
  bool d = sel_dirty;
  sel_dirty = false;
  return d;
}
