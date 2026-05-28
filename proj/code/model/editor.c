#include "editor.h"
#include <string.h>

static char lines[MAX_LINES][MAX_COLS];
static int cursor_row = 0;
static int cursor_col = 0;
static int row_count = 1;

int editor_init() {
  memset(lines, 0, sizeof(lines));
  cursor_row = 0;
  cursor_col = 0;
  row_count = 1;
  return 0;
}

void editor_cleanup() {}

void editor_insert_char(char c) {
  if (c == '\n') {
    if (cursor_row >= MAX_LINES - 1) return;
    cursor_row++;
    cursor_col = 0;
    if (cursor_row >= row_count) row_count = cursor_row + 1;
    return;
  }

  if (cursor_col >= MAX_COLS - 1) return;
  lines[cursor_row][cursor_col] = c;
  cursor_col++;
}

void editor_delete_char() {
  if (cursor_col > 0) {
    cursor_col--;
    lines[cursor_row][cursor_col] = '\0';
  } else if (cursor_row > 0) {
    cursor_row--;
    cursor_col = strlen(lines[cursor_row]);
  }
}

const char *editor_get_line(int row) {
  if (row < 0 || row >= MAX_LINES) return "";
  return lines[row];
}

int editor_get_row_count() { return row_count; }
int editor_get_cursor_row() { return cursor_row; }
int editor_get_cursor_col() { return cursor_col; }
