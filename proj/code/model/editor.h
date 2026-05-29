#pragma once

#define MAX_LINES 48
#define MAX_COLS 256

int editor_init();
void editor_cleanup();
void editor_insert_char(char c);
void editor_delete_char();
void editor_delete_word();
void editor_move_left();
void editor_move_right();
void editor_move_up();
void editor_move_down();
void editor_move_word_left();
void editor_move_word_right();
const char *editor_get_line(int row);
int editor_get_row_count();
int editor_get_cursor_row();
int editor_get_cursor_col();
