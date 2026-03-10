#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void(timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  // TIMER_RB_CMD -> Tells that this is a READ-BACK command
  // TIMER_RB_COUNT_ -> Tells that we dont want the count
  // TIMER_RB_SEL(timer) -> Selects timer
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, cmd) != 0)
    return 1;

  if (util_sys_inb(TIMER_0 + timer, st) != 0)
    return 1;
  

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val status_val;

  switch(field) {
    case tsf_all:
      status_val.byte = st;
      break;

    case tsf_initial: {
      status_val.in_mode = (st >> 4) & 0x03;
      break;
    }

    case tsf_mode: {
      uint8_t mode = (st >> 1) & 0x07;
      status_val.count_mode = mode;
      break;
    }

    case tsf_base: {
      uint8_t base = st & 0x01;
      status_val.bcd = base;
      break;
    }
  }

  timer_print_config(timer, field, status_val);

  return 0;
}
