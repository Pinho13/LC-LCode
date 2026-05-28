#pragma once

#define MAX_LINES 48
#define MAX_COLS 128

int editor_init();
void editor_cleanup();
void editor_insert_char(char c);
void editor_delete_char();
const char *editor_get_line(int row);
int editor_get_row_count();
int editor_get_cursor_row();
int editor_get_cursor_col();
