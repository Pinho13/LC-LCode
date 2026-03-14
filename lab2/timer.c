#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

#include "i8254.h"

uint32_t tc = 0; 
static int hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || freq > TIMER_FREQ) return 1; 
  if (timer > 2) return 1; 
  uint8_t st; 
  if (timer_get_conf(timer, &st) != 0) return 1;
  uint8_t cw = timer << 6; 
  cw = cw | TIMER_LSB_MSB | (st & 0x0F);
  if (sys_outb(TIMER_CTRL, cw) != 0) return 1;
  uint16_t div = TIMER_FREQ / freq;

  uint8_t lsb, msb; 
  if (util_get_LSB(div,&lsb) != 0) return 1;
  if (util_get_MSB(div, &msb) != 0) return 1;

  //First LSB first 
  if (sys_outb(TIMER_0 + timer, lsb) != 0) return 1;
  if (sys_outb(TIMER_0 + timer, msb) != 0) return 1;

  return 0; 
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1; 
  hook_id = TIMER0_IRQ;
  if( sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1;
  *bit_no = hook_id;
  return 0;  
}

int (timer_unsubscribe_int)() {
  return (sys_irqrmpolicy(&hook_id));
}

void (timer_int_handler)() {
  tc++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer > 2) {
      return 1;
    }

  uint8_t rb_command = 0;
  //Selecionar Bits Read Back Command
  rb_command |= TIMER_RB_CMD;

  //Selecionar bit para não ser count
  rb_command |= TIMER_RB_COUNT_;

  //Selecionar bits relativos ao timer
  rb_command |= TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, rb_command) != 0) {
      return 1;
    }

  if (util_sys_inb(TIMER_0 + timer, st) != 0) {
    return 1;
  }

  return 0;

}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  union timer_status_field_val val;
  uint8_t mode;
  switch(field){
    case tsf_all:{
      val.byte = st; break;
    }
    case tsf_initial:{
      mode = st & TIMER_SW_TypeAccess;
      mode = mode >> 4;
      val.in_mode = (enum timer_init) mode;
      break;
    } 
    case tsf_base:{
      val.bcd = st & TIMER_SW_BCD;
      break;
    }
    case tsf_mode:{
      mode  = st & TIMER_SW_ProgrammedMode;
      mode = mode >> 1; 
      if (mode > 5) mode -= 4; 
      val.count_mode = mode; 
      break; 
    }
    default: return 1; 
  }
  return timer_print_config(timer, field, val);
}
