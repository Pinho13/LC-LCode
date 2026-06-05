#pragma once
#include <stdbool.h>

typedef enum {
  EDITOR_OK,
  EDITOR_ERR_NO_CLIPBOARD,
  EDITOR_ERR_ALLOC_FAILED
} EditorResult;

typedef struct {
  char *buf;
  int len;
  int cap;
} Line;

int editor_init();
void editor_cleanup();

void editor_set_viewport(int rows, int cols);
void editor_scroll_by(int drow, int dcol);
void editor_set_cursor(int row, int col);
bool editor_consume_scroll_dirty();

EditorResult editor_insert_char(char c);
EditorResult editor_delete_char();
EditorResult editor_delete_word();

void editor_move_left();
void editor_move_right();
void editor_move_up();
void editor_move_down();
void editor_move_word_left();
void editor_move_word_right();
void editor_move_home();
void editor_move_end();

const char *editor_get_line(int row);
int editor_get_line_len(int row);
int editor_get_row_count();

int editor_get_cursor_row();
int editor_get_cursor_col();

int editor_get_scroll_row();
int editor_get_scroll_col();

EditorResult editor_delete_selection();
void editor_sel_set_anchor();
void editor_sel_clear();
bool editor_sel_is_active();
void editor_sel_get_range(int *start_row, int *start_col, int *end_row, int *end_col);
bool editor_consume_sel_dirty();

void editor_copy_selection();
EditorResult editor_paste();

EditorResult editor_load_line(const char *text, int len);
void editor_load_finalize();

// Remote 

void editor_set_remote_cursor(int row, int col);

/**
 * @brief Inserts a character at the remote cursor position.
 * 
 * Manages the memory reallocation if the line needs to grow, and pushes the text right.
 * If a newline character is inserted, it splits the current line dynamically.
 * The implementation assumes that the cursor is always within the bounds of the document and it is very similar to editor_insert_char, without updating the local cursor position.
 * 
 * @param c The character to insert.
 * @return EDITOR_OK on success, or EDITOR_ERR_ALLOC_FAILED on memory error.
 */
EditorResult editor_remote_insert_char(char c);

/**
 * @brief Deletes the character immediately before the remote cursor position.
 * 
 * If the remote cursor is at the beginning of a line, it merges the current line
 * with the previous line, performing a `memmove` to collapse the rows array.
 * Works similarly to editor_delete_char but without updating the local cursor position.
 * 
 * @return EDITOR_OK on success, or EDITOR_ERR_ALLOC_FAILED on memory error.
 */
EditorResult editor_remote_delete_char();

/**
 * @brief Prepares the text buffer for a block replacement.
 * 
 * This is the core of the remote block processing algorithm. It handles the structural
 * memory shifts (`memmove` on the lines array) required when multiple lines are 
 * deleted or inserted at once. For example, during a Paste or Cut operation, it 
 * dynamically expands or shrinks the total row count of the buffer.
 * 
 * @param start_row The index of the first row to be affected.
 * @param deleted_count Number of lines to remove from the buffer.
 * @param inserted_count Number of empty lines to allocate and make room for.
 * @return EDITOR_OK on success, or EDITOR_ERR_ALLOC_FAILED on memory error.
 */
EditorResult editor_remote_replace_block(int start_row, int deleted_count, int inserted_count);

/**
 * @brief Overwrites a specific line with new text received from the network.
 * 
 * Used immediately after `editor_remote_replace_block` to fill the newly created
 * empty lines with the actual payload text. Reallocates the internal buffer if the 
 * new text is larger than the previous capacity.
 * 
 * @param row The index of the line to update.
 * @param text The new string to be copied into the line.
 * @param len The length of the new string.
 * @return EDITOR_OK on success, or EDITOR_ERR_ALLOC_FAILED on memory error.
 */
EditorResult editor_remote_update_line(int row, const char *text, int len);
int editor_get_remote_cursor_row();
int editor_get_remote_cursor_col();

