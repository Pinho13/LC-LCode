#include <lcom/lcf.h>

#include "ih.h"
#include "fw/common/utils.h"
#include "render_flag.h"

#define SCANCODE_ESC 0x01
#define SCANCODE_Q 0x10
#define SCANCODE_LCTRL 0x1D
#define SCANCODE_RCTRL 0x1D

static int blink_tick = 0;
static bool ctrl_pressed = false;
static bool quit_flag = false;

static uint8_t irq_timer = 0, irq_keyboard = 0, irq_mouse = 0;
packet_scancode ps = {
  .two_byte = false,
  .make = false,
  .size = 0,
  .bytes = {0, 0}
};

int subscribe_interrupts() {
  uint8_t bit_no; // used once per interrupt, no need to create one per driver

  if (timer_subscribe_int(&bit_no) != OK) {
    return fail(ERR_TIMER, "subscribe_interrupts: unable to subscribe timer interrupt");
  }
  irq_timer = BIT(bit_no);

  if (keyboard_subscribe_int(&bit_no) != OK) {
    timer_unsubscribe_int();
    return fail(ERR_KEYBOARD, "subscribe_interrupts: unable to subscribe keyboard interrupt");
  }
  irq_keyboard = BIT(bit_no);

  if (mouse_subscribe_int(&bit_no) != OK) {
    timer_unsubscribe_int();
    keyboard_unsubscribe_int();
    return fail(ERR_MOUSE, "subscribe_interrupts: unable to subscribe mouse interrupt");
  }
  if (my_mouse_enable_data_reporting() != OK) {
    timer_unsubscribe_int();
    keyboard_unsubscribe_int();
    mouse_unsubscribe_int();
    return fail(ERR_MOUSE, "subscribe_interrupts: unable to enable data reporting");
  }
  irq_mouse = BIT(bit_no);

  return 0;
}

int unsubscribe_interrupts() {
  int errors = 0;

  if (mouse_unsubscribe_int() != OK) {
    errors = 1;
    fail(ERR_MOUSE, "unsubscribe_interrupts: unable to unsubscribe mouse interrupt");
  }

  if (keyboard_unsubscribe_int() != OK) {
    errors = 1;
    fail(ERR_KEYBOARD, "unsubscribe_interrupts: unable to unsubscribe keyboard interrupt");
  }

  if (timer_unsubscribe_int() != OK) {
    errors = 1;
    fail(ERR_TIMER, "unsubscribe_interrupts: unable to unsubscribe timer interrupt");
  }

  if (my_mouse_disable_data_reporting() != OK) {
    errors = 1;
    fail(ERR_MOUSE, "unsubscribe_interrupts: unable to disable mouse data reporting");
  }

  return errors;
}

void timer_handler() {
  timer_int_handler();
  blink_tick++;
  if (blink_tick % BLINK_TICKS == 0)
    set_dirty(DIRTY_CURSOR);
}

bool get_quit() { return quit_flag; }

void keyboard_handler() {
  keyboard_ih();

  if (build_scancode(&ps) != OK) return;

  if (ps.two_byte) {
    if ((ps.bytes[1] & 0x7F) == SCANCODE_RCTRL)
      ctrl_pressed = ps.make;
    return;
  }

  if (ps.bytes[0] == SCANCODE_LCTRL) {
    ctrl_pressed = ps.make;
    return;
  }

  if (!ps.make) return;

  if (ps.bytes[0] == SCANCODE_ESC || (ps.bytes[0] == SCANCODE_Q && ctrl_pressed))
    quit_flag = true;

  keyboard_print_scancode(ps);
}

void mouse_handler() {
  mouse_ih();

  if (is_packet_ready()) {
    struct packet pp;
  
    if (build_packet(&pp) != OK) {
      fail(ERR_MOUSE, "mouse_example: unable to build packet");
      return;
    }
    mouse_print_packet(&pp);
  }
}

void interrupts_handler(uint32_t irq_mask) {
  if (irq_mask & irq_timer) {
    timer_handler();
  }
  if (irq_mask & irq_keyboard) {
    keyboard_handler();
  }
  if (irq_mask & irq_mouse) {
    mouse_handler();
  }
}

