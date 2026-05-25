#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  uint32_t irq_set = BIT(bit_no);
  int ipc_status;
  message msg;

  uint8_t bytes[2];
  uint8_t size = 0;
  bool done = false;

  while (!done) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if (kbc_get_error()) {
              size = 0;
              break;
            }

            uint8_t byte = kbc_get_scancode();
            bytes[size++] = byte;

            if (byte != TWO_BYTE_PREFIX) {
              bool make = !(byte & BREAK_BIT);
              kbd_print_scancode(make, size, bytes);

              if (!make && byte == ESC_BREAK) {
                done = true;
              }

              size = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}

int(kbd_test_poll)() {
  uint8_t bytes[2];
  uint8_t size = 0;
  bool done = false;

  while (!done) {
    uint8_t st;
    if (util_sys_inb(KBC_STATUS_REG, &st) != 0) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if (!(st & KBC_ST_OBF)) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    uint8_t byte;
    if (util_sys_inb(KBC_OUTPUT_BUF, &byte) != 0) {
      size = 0;
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if (st & KBC_ST_AUX) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if (st & (KBC_ST_TO_ERR | KBC_ST_PAR_ERR)) {
      size = 0;
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    bytes[size++] = byte;

    if (byte == TWO_BYTE_PREFIX) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    bool make = !(byte & BREAK_BIT);
    kbd_print_scancode(make, size, bytes);

    if (!make && byte == ESC_BREAK) {
      done = true;
    }

    size = 0;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return kbc_enable_int();
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) return 1;
  uint32_t kbd_irq_set = BIT(kbd_bit_no);

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) != 0) {
    kbd_unsubscribe_int();
    return 1;
  }
  uint32_t timer_irq_set = BIT(timer_bit_no);

  uint8_t bytes[2];
  uint8_t size = 0;
  bool done = false;
  uint32_t idle_ticks = 0;
  uint32_t max_idle_ticks = (uint32_t) n * 60;

  int ipc_status;
  message msg;

  while (!done) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_irq_set) {
            kbc_ih();

            idle_ticks = 0;

            if (kbc_get_error()) {
              size = 0;
            } else {
              uint8_t byte = kbc_get_scancode();

              if (size >= 2) size = 0;
              bytes[size++] = byte;

              if (byte != TWO_BYTE_PREFIX) {
                bool make = !(byte & BREAK_BIT);
                kbd_print_scancode(make, size, bytes);

                if (!make && byte == ESC_BREAK) {
                  done = true;
                }

                size = 0;
              }
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();

            if (!done) {
              idle_ticks++;
              if (n > 0 && idle_ticks >= max_idle_ticks) {
                done = true;
              }
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (timer_unsubscribe_int() != 0) {
    kbd_unsubscribe_int();
    return 1;
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}
