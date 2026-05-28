#include "keyboard.h"
#include "commands.h"
#include "fw/drivers/keyboard.h"

#define SCANCODE_ESC 0x01
#define SCANCODE_BACKSPACE 0x0E
#define SCANCODE_ENTER 0x1C
#define SCANCODE_LCTRL 0x1D
#define SCANCODE_RCTRL 0x1D
#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x36
#define SCANCODE_UP     0x48
#define SCANCODE_DOWN   0x50
#define SCANCODE_LEFT   0x4B
#define SCANCODE_RIGHT  0x4D

static const char sc_lower[58] = {
  0,    0,   '1', '2', '3', '4', '5', '6',  /* 0x00-0x07 */
  '7', '8', '9', '0','\'',  0,    0,  '\t', /* 0x08-0x0F */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',  /* 0x10-0x17 */
  'o', 'p', '+',  0,    0,    0,  'a', 's', /* 0x18-0x1F */
  'd', 'f', 'g', 'h', 'j', 'k', 'l',  0,   /* 0x20-0x27 */
   0,  '\\', 0,  '~', 'z', 'x', 'c', 'v',  /* 0x28-0x2F */
  'b', 'n', 'm', ',', '.', '-',  0,    0,   /* 0x30-0x37 */
  0,   ' '                                  /* 0x38-0x39 */
};

static const char sc_upper[58] = {
  0,    0,   '!', '"', '#', '$', '%', '&',  /* 0x00-0x07 */
  '/', '(', ')', '=', '?',  0,    0,  '\t', /* 0x08-0x0F */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  /* 0x10-0x17 */
  'O', 'P', '*',  0,    0,    0,  'A', 'S', /* 0x18-0x1F */
  'D', 'F', 'G', 'H', 'J', 'K', 'L',  0,   /* 0x20-0x27 */
   0,  '|',  0,  '^', 'Z', 'X', 'C', 'V',  /* 0x28-0x2F */
  'B', 'N', 'M', ';', ':', '_',  0,    0,   /* 0x30-0x37 */
  0,   ' '                                  /* 0x38-0x39 */
};

static packet_scancode ps = {
  .two_byte = false, .make = false, .size = 0, .bytes = {0, 0}
};

static bool ctrl_pressed = false;
static bool shift_pressed = false;

void keyboard_process() {
  keyboard_ih();
  if (build_scancode(&ps) != OK) return;

  if (ps.two_byte) return;

  if (ps.size == 2) {
    uint8_t code2 = ps.bytes[1] & 0x7F;
    if (code2 == SCANCODE_RCTRL) { ctrl_pressed = ps.make; return; }
    if (!ps.make) return;
    KeyEvent ev = {
      .c = 0, .ctrl = ctrl_pressed, .shift = shift_pressed,
      .backspace = false, .enter = false, .escape = false, .dir = DIR_NONE
    };
    if (code2 == SCANCODE_LEFT) ev.dir = DIR_LEFT;
    else if (code2 == SCANCODE_RIGHT) ev.dir = DIR_RIGHT;
    else if (code2 == SCANCODE_UP) ev.dir = DIR_UP;
    else if (code2 == SCANCODE_DOWN) ev.dir = DIR_DOWN;
    if (ev.dir != DIR_NONE) commands_dispatch(ev);
    return;
  }

  uint8_t code = ps.bytes[0] & 0x7F;

  if (code == SCANCODE_LCTRL) { ctrl_pressed = ps.make; return; }
  if (code == SCANCODE_LSHIFT || code == SCANCODE_RSHIFT) { shift_pressed = ps.make; return; }

  if (!ps.make) return;

  KeyEvent ev = {
    .c = 0, .ctrl = ctrl_pressed, .shift = shift_pressed,
    .backspace = false, .enter = false, .escape = false, .dir = DIR_NONE
  };

  if (code == SCANCODE_ESC) { ev.escape = true; commands_dispatch(ev); return; }
  if (code == SCANCODE_BACKSPACE) { ev.backspace = true; commands_dispatch(ev); return; }
  if (code == SCANCODE_ENTER) { ev.enter = true; commands_dispatch(ev); return; }

  if (code < 58) {
    char c = shift_pressed ? sc_upper[code] : sc_lower[code];
    if (c) { ev.c = c; commands_dispatch(ev); }
  }
}
