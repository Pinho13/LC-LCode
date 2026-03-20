#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
static uint32_t timer_int_counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer > 2) return 1;
  if (freq < 1 || freq > TIMER_FREQ) return 1;

  uint8_t conf;
  if (timer_get_conf(timer, &conf) != 0) return 1;

  /* Preserve bits 3:0 (operating mode + BCD), set timer select and LSB_MSB */
  uint8_t ctrl = 0;
  switch (timer) {
    case 0: ctrl |= TIMER_SEL0; break;
    case 1: ctrl |= TIMER_SEL1; break;
    case 2: ctrl |= TIMER_SEL2; break;
  }
  ctrl |= TIMER_LSB_MSB;
  ctrl |= (conf & 0x0F); /* keep lower 4 bits: mode and BCD */

  if (sys_outb(TIMER_CTRL, ctrl) != OK) return 1;

  uint16_t div = (uint16_t)(TIMER_FREQ / freq);

  uint8_t lsb, msb;
  if (util_get_LSB(div, &lsb) != 0) return 1;
  if (util_get_MSB(div, &msb) != 0) return 1;

  uint8_t timer_port = TIMER_0 + timer;
  if (sys_outb(timer_port, lsb) != OK) return 1;
  if (sys_outb(timer_port, msb) != OK) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) return 1;
  return 0;
}

void (timer_int_handler)() {
  timer_int_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL) return 1;
  if (timer > 2) return 1;

  /* Read-Back command: bit5=1 (do NOT latch count), bit4=0 (DO latch status) */
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, cmd) != OK) return 1;

  uint8_t timer_port = TIMER_0 + timer;
  if (util_sys_inb(timer_port, st) != OK) return 1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val val;

  switch (field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial: {
      uint8_t init = (st >> 4) & 0x03;
      switch (init) {
        case 1: val.in_mode = LSB_only; break;
        case 2: val.in_mode = MSB_only; break;
        case 3: val.in_mode = MSB_after_LSB; break;
        default: val.in_mode = INVAL_val; break;
      }
      break;
    }
    case tsf_mode: {
      uint8_t mode = (st >> 1) & 0x07;
      if (mode == 6) mode = 2;
      if (mode == 7) mode = 3;
      val.count_mode = mode;
      break;
    }
    case tsf_base:
      val.bcd = (st & TIMER_BCD) ? true : false;
      break;
    default:
      return 1;
  }

  if (timer_print_config(timer, field, val) != OK) return 1;

  return 0;
}
