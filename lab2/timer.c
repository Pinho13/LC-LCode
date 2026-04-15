#include "timer.h"

static int hook_id;
static uint32_t contador=0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t status;
  if (timer_get_conf(timer,&status) != 0) return 1;
  uint8_t aux = (status & 0x0F) | TIMER_LSB_MSB;
  
  if (timer == 0) aux |= TIMER_SEL0;
  else if (timer == 1) aux |= TIMER_SEL1;
  else if (timer == 2) aux |= TIMER_SEL2;
  else return 1;

  if (sys_outb(TIMER_CTRL, aux) != 0) return 1;

  int port=TIMER_0+timer;
  uint16_t divider = TIMER_FREQ/freq;
  uint8_t lsb,msb;
  
  if(util_get_LSB(divider,&lsb)!=0) return 1;
  if(util_get_MSB(divider,&msb)!=0) return 1;

  if (sys_outb(port, lsb) != 0) return 1;
  if (sys_outb(port, msb) != 0) return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id)!=0) return 1;
  return 0;
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id)!=0) return 1;
  return 0;
}

void(timer_int_handler)() {
  contador++;
}

uint32_t get_counter(){
  return contador;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, command) != 0)
    return 1;
  int port=TIMER_0+timer;
  if (util_sys_inb(port, st) != 0)
    return 1;
  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val conf;
  uint8_t mode;
  switch (field) {
    case tsf_all:
      conf.byte=st;
      break;
    case tsf_initial:
      mode = (st >> 4) & 0x03;
      if (mode == 1) conf.in_mode=LSB_only;
      else if (mode == 2) conf.in_mode=MSB_only;
      else if (mode == 3) conf.in_mode=MSB_after_LSB;
      else conf.in_mode=INVAL_val;
      break;
    case tsf_mode:
      mode = (st >> 1) & 0x07;
      mode = mode == 6 ? 2 : mode;
      mode = mode == 7 ? 3 : mode;
      conf.count_mode=mode;
      break;
    case tsf_base:
      mode = st & 0x01;
      conf.bcd=mode;
      break;
    default:
      return 1;
  }
  if(timer_print_config(timer, field, conf)!=0) return 1;
  return 0;
}
