#include "controller/input/keyboard.h"
#include "controller/input/events.h"

// SCANCODES

#define SCANCODE_ESC 0x01
#define SCANCODE_BACKSPACE 0x0E
#define SCANCODE_ENTER 0x1C
#define SCANCODE_LCTRL 0x1D
#define SCANCODE_RCTRL 0x1D
#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x36
#define SCANCODE_RALT 0x38

#define SCANCODE_UP 0x48
#define SCANCODE_DOWN 0x50
#define SCANCODE_LEFT 0x4B
#define SCANCODE_RIGHT 0x4D
#define SCANCODE_HOME 0x47
#define SCANCODE_END 0x4F

// KEYMAPS

static const char sc_lower[58] = {
    0,   0,    '1', '2', '3',  '4', '5', '6',  /* 0x00-0x07 */
    '7', '8',  '9', '0', '\'', 0,   0,   '\t', /* 0x08-0x0F */
    'q', 'w',  'e', 'r', 't',  'y', 'u', 'i',  /* 0x10-0x17 */
    'o', 'p',  '+', 0,   0,    0,   'a', 's',  /* 0x18-0x1F */
    'd', 'f',  'g', 'h', 'j',  'k', 'l', 0,    /* 0x20-0x27 */
    0,   '\\', 0,   '~', 'z',  'x', 'c', 'v',  /* 0x28-0x2F */
    'b', 'n',  'm', ',', '.',  '-', 0,   0,    /* 0x30-0x37 */
    0,   ' '                                   /* 0x38-0x39 */
};

static const char sc_upper[58] = {
    0,   0,   '!', '"', '#', '$', '%', '&',  /* 0x00-0x07 */
    '/', '(', ')', '=', '?', 0,   0,   '\t', /* 0x08-0x0F */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  /* 0x10-0x17 */
    'O', 'P', '*', 0,   0,   0,   'A', 'S',  /* 0x18-0x1F */
    'D', 'F', 'G', 'H', 'J', 'K', 'L', 0,    /* 0x20-0x27 */
    0,   '|', 0,   '^', 'Z', 'X', 'C', 'V',  /* 0x28-0x2F */
    'B', 'N', 'M', ';', ':', '_', 0,   0,    /* 0x30-0x37 */
    0,   ' '                                 /* 0x38-0x39 */
};

static const char sc_altgr[58] = {
    0,   0,    '1', '@', '3',  '4', '5', '6',  /* 0x00-0x07 */
    '{', '[',  ']', '}', '\'', 0,   0,   '\t', /* 0x08-0x0F */
    'q', 'w',  'e', 'r', 't',  'y', 'u', 'i',  /* 0x10-0x17 */
    'o', 'p',  '+', 0,   0,    0,   'a', 's',  /* 0x18-0x1F */
    'd', 'f',  'g', 'h', 'j',  'k', 'l', 0,    /* 0x20-0x27 */
    0,   '\\', 0,   '~', 'z',  'x', 'c', 'v',  /* 0x28-0x2F */
    'b', 'n',  'm', ',', '.',  '-', 0,   0,    /* 0x30-0x37 */
    0,   ' '                                   /* 0x38-0x39 */
};

// KEYBOARD STATE

typedef struct {
  bool ctrl;
  bool shift;
  bool altgr;
} KeyboardState;

static KeyboardState kb = {.ctrl = false, .shift = false, .altgr = false};

// HELPERS

static void push_event(KeyEvent ev) {
  InputEvent iev = {.type = INPUT_EVENT_KEY, .data.key = ev};
  input_event_push(iev);
}

static KeyEvent make_event(void) {
  return (KeyEvent){.c = 0, .ctrl = kb.ctrl, .shift = kb.shift, .altgr = kb.altgr,
                    .backspace = false, .enter = false, .escape = false, .dir = DIR_NONE};
}

static char translate_char(uint8_t code) {
  if (code >= 58)
    return 0;

  if (kb.altgr)
    return sc_altgr[code];
  if (kb.shift)
    return sc_upper[code];

  return sc_lower[code];
}

static void handle_extended(packet_scancode ps) {
  uint8_t code = ps.bytes[1] & 0x7F;

  if (code == SCANCODE_RCTRL) {
    kb.ctrl = ps.make;
    return;
  }

  if (code == SCANCODE_RALT) {
    kb.altgr = ps.make;
    return;
  }

  if (!ps.make)
    return;

  KeyEvent ev = make_event();

  switch (code) {
  case SCANCODE_LEFT:
    ev.dir = DIR_LEFT;
    break;
  case SCANCODE_RIGHT:
    ev.dir = DIR_RIGHT;
    break;
  case SCANCODE_UP:
    ev.dir = DIR_UP;
    break;
  case SCANCODE_DOWN:
    ev.dir = DIR_DOWN;
    break;
  case SCANCODE_HOME:
    ev.dir = DIR_HOME;
    break;
  case SCANCODE_END:
    ev.dir = DIR_END;
    break;
  default:
    return;
  }

  push_event(ev);
}

// MAIN FUNCTION

void keyboard_process(packet_scancode ps) {

  if (ps.size == 2) {
    handle_extended(ps);
    return;
  }

  uint8_t code = ps.bytes[0] & 0x7F;

  if (code == SCANCODE_LCTRL) {
    kb.ctrl = ps.make;
    return;
  }

  if (code == SCANCODE_LSHIFT || code == SCANCODE_RSHIFT) {
    kb.shift = ps.make;
    return;
  }

  if (!ps.make)
    return;

  KeyEvent ev = make_event();

  switch (code) {
    case SCANCODE_ESC:
      ev.escape = true;
      break;

    case SCANCODE_BACKSPACE:
      ev.backspace = true;
      break;

    case SCANCODE_ENTER:
      ev.enter = true;
      break;

    default:
      ev.c = translate_char(code);
      break;
  }

  if (ev.escape || ev.backspace || ev.enter || ev.dir != DIR_NONE || ev.c) {
    push_event(ev);
  }
}
