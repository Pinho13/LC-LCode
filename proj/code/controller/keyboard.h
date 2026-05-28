#pragma once

#include <lcom/lcf.h>

typedef struct {
  char c;
  bool ctrl;
  bool shift;
  bool backspace;
  bool enter;
  bool escape;
} KeyEvent;

void keyboard_process();
