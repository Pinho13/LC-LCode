#pragma once

#include <lcom/lcf.h>

#define CMD_BUF_SIZE 128

typedef enum { MODE_EDITOR, MODE_COMMAND } EditorMode;

void command_bar_init(const char *filename);
EditorMode command_bar_get_mode();
const char *command_bar_get_filename();
void command_bar_set_filename(const char *name);
void command_bar_start();
void command_bar_start_prefill(const char *text);
void command_bar_cancel();
const char *command_bar_get_input();
void command_bar_insert(char c);
void command_bar_delete();
const char *command_bar_commit();
