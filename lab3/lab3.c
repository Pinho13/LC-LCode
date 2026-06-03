#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"

/* Shared state for scancode assembly across the three functions */
static uint8_t bytes[2];
static bool    two_byte = false;

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

/* ---------------------------------------------------------------------------
 * kbd_test_scan  --  read scancodes via KBC interrupts (IRQ 1)
 * Exits when the ESC breakcode (0x81) is received.
 * --------------------------------------------------------------------------- */
int (kbd_test_scan)(void) {
  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != 0)
    return 1;

  uint32_t irq_set = BIT(bit_no);
  int ipc_status;
  message msg;
  int r;

  while (1) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            uint8_t byte = get_scancode_byte();

            /* First byte of a two-byte scancode */
            if (byte == TWO_BYTE) {
              two_byte = true;
              bytes[0] = byte;
              continue;
            }

            uint8_t size;
            if (two_byte) {
              bytes[1] = byte;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = byte;
              size = 1;
            }

            /* MSB of last byte: 1 = breakcode, 0 = makecode */
            bool make = !(byte & BIT(7));
            kbd_print_scancode(make, size, bytes);

            /* Terminate on ESC break (single-byte 0x81) */
            if (size == 1 && bytes[0] == ESC_BREAK) {
              kbd_unsubscribe_int();
              return 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  return 0;
}

/* ---------------------------------------------------------------------------
 * kbd_test_poll  --  read scancodes via polling (no interrupts)
 *
 * lcf_start() disables KBC keyboard interrupts before calling this function,
 * so polling is safe.  Before returning we must re-enable interrupts by
 * writing a new Command Byte with the INT bit set.
 * --------------------------------------------------------------------------- */
int (kbd_test_poll)(void) {
  uint8_t status, data;
  two_byte = false;

  while (1) {
    if (util_sys_inb(KBC_STATUS_REG, &status) != OK) {
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    /* No data yet - wait */
    if (!(status & OBF)) {
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    /* Always read to clear the buffer, even on error */
    if (util_sys_inb(KBC_OUT_BUF, &data) != OK) {
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    /* Discard on communication error */
    if (status & (PAR_ERR | TO_ERR)) {
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    /* First byte of a two-byte scancode */
    if (data == TWO_BYTE) {
      two_byte = true;
      bytes[0] = data;
      continue;
    }

    uint8_t size;
    if (two_byte) {
      bytes[1] = data;
      size = 2;
      two_byte = false;
    } else {
      bytes[0] = data;
      size = 1;
    }

    bool make = !(data & BIT(7));
    kbd_print_scancode(make, size, bytes);

    /* Terminate on ESC break */
    if (size == 1 && bytes[0] == ESC_BREAK)
      break;

    tickdelay(micros_to_ticks(20000));
  }

  /* Re-enable KBC keyboard interrupts:
   * 1. Wait for IBF to be clear
   * 2. Send READ_CMD (0x20) to command register
   * 3. Read current Command Byte from output buffer
   * 4. Set INT bit (bit 0)
   * 5. Send WRITE_CMD (0x60) to command register
   * 6. Write updated Command Byte to input buffer (0x60)
   */
  uint8_t cmd_byte, st;

  do {
    util_sys_inb(KBC_STATUS_REG, &st);
  } while (st & IBF);

  if (sys_outb(KBC_STATUS_REG, KBC_READ_CMD) != OK)
    return 1;

  if (util_sys_inb(KBC_OUT_BUF, &cmd_byte) != OK)
    return 1;

  cmd_byte |= BIT(0); /* set INT: enable keyboard interrupts */

  do {
    util_sys_inb(KBC_STATUS_REG, &st);
  } while (st & IBF);

  if (sys_outb(KBC_STATUS_REG, KBC_WRITE_CMD) != OK)
    return 1;

  do {
    util_sys_inb(KBC_STATUS_REG, &st);
  } while (st & IBF);

  if (sys_outb(KBC_OUT_BUF, cmd_byte) != OK)
    return 1;

  return 0;
}

/* ---------------------------------------------------------------------------
 * kbd_test_timed_scan  --  interrupts from both KBC (IRQ1) and Timer (IRQ0)
 * Exits on ESC break OR after n seconds of inactivity.
 * Timer runs at 60 Hz in MINIX, so n seconds = n * 60 ticks.
 * --------------------------------------------------------------------------- */
int (kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbd_bit_no, timer_bit_no;

  if (kbd_subscribe_int(&kbd_bit_no) != 0)
    return 1;
  if (timer_subscribe_int(&timer_bit_no) != 0) {
    kbd_unsubscribe_int();
    return 1;
  }

  uint32_t kbd_irq_set   = BIT(kbd_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);
  uint32_t timer_ticks   = 0;

  two_byte = false;

  int ipc_status;
  message msg;
  int r;

  while (1) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_irq_set) {
            kbc_ih();

            /* Any keyboard activity resets the inactivity timer */
            timer_ticks = 0;

            uint8_t byte = get_scancode_byte();

            if (byte == TWO_BYTE) {
              two_byte = true;
              bytes[0] = byte;
              break; /* break out of switch, back to driver_receive */
            }

            uint8_t size;
            if (two_byte) {
              bytes[1] = byte;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = byte;
              size = 1;
            }

            bool make = !(byte & BIT(7));
            kbd_print_scancode(make, size, bytes);

            if (size == 1 && bytes[0] == ESC_BREAK) {
              kbd_unsubscribe_int();
              timer_unsubscribe_int();
              return 0;
            }
          }

          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            timer_ticks++;
            if (timer_ticks >= (uint32_t)(n * 60)) {
              kbd_unsubscribe_int();
              timer_unsubscribe_int();
              return 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  timer_unsubscribe_int();
  return 0;
}
