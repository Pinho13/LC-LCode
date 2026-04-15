#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "keyboard.h"

bool error=false;
uint32_t get_counter();//tive que dar esta declaração para conseguir compilar

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
  uint8_t hook_id = 1;
  uint32_t kbd_set = BIT(hook_id);
  
  if (keyboard_subscribe_int(&hook_id)!=0) return 1;

  int r, ipc_status;
  message msg;

  bool make;
  uint8_t size=0;
  uint8_t bytes[2];

  bool running = true;
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_set) {
            
            kbc_ih(); // 1. O handler lê o byte do teclado

            if (error) continue;

            uint8_t scancode = get_scancode();
            bytes[size]=scancode;
            size++;
            
            if (scancode==0xE0) continue;
      
            //chegando aqui então o bytes já está prenchido quer por 1 ou 2 bytes

            if (scancode&BREAKCODE) make=false;
            else make=true;
            
            kbd_print_scancode(make, size, bytes);

            if (scancode == 0x81){
              running = false;
            } 
            
            size=0;

          }
          break;
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int()!=0) return 1;

  return 0;
}

int(kbd_test_poll)() {
  bool make;
  uint8_t size=0;
  uint8_t bytes[2];
  bool running=true;

  while (running){
    error=false;
    kbc_ih(); 

    if (error){
      tickdelay(micros_to_ticks(20000));
      continue;
    } 

    uint8_t scancode = get_scancode();
    bytes[size]=scancode;
    size++;
            
    if (scancode==0xE0) continue;
      
    //chegando aqui então o bytes já está prenchido quer por 1 ou 2 bytes

    if (scancode&BREAKCODE) make=false;
    else make=true;
            
    kbd_print_scancode(make, size, bytes);

    if (scancode == 0x81){
      running = false;
    } 
            
    size=0;
    tickdelay(micros_to_ticks(20000));
  }
  
  //voltar a logar as interrupções
  uint8_t stat;
  uint8_t cmd;
  while ( 1 ) {
    if (util_sys_inb(KBC_STATUS_REG, &stat) != 0) return 1;
    if( (stat & KBC_ST_IBF) == 0 ) {//se tiver vazio entra aqui
        if(sys_outb(KBC_STATUS_REG, RD_CMD)!=0)return 1;
        tickdelay(micros_to_ticks(20000));
        if (util_sys_inb(KBC_DATA_REG, &cmd) != 0) return 1;
        cmd = cmd | BIT(0);
        if(sys_outb(KBC_STATUS_REG, WR_CMD)!=0)return 1;
        if(sys_outb(KBC_DATA_REG, cmd)!=0)return 1;
        return 0;
    }
    tickdelay(micros_to_ticks(20000));
  }
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  uint8_t hook_id_kbc = 1, hook_id_timer=0;
  uint32_t timer_set = BIT(hook_id_timer);
  uint32_t kbd_set = BIT(hook_id_kbc);
  
  if (keyboard_subscribe_int(&hook_id_kbc)!=0) return 1;
  if (timer_subscribe_int(&hook_id_timer)!=0) return 1;

  int r, ipc_status;
  message msg;

  bool make;
  uint8_t size=0;
  uint8_t bytes[2];

  bool running = true;
  uint32_t last;
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_set){
            timer_int_handler();
            if (get_counter()-last >= (uint32_t)(n * 60)) {
              running = false;
            }
          }
          if (msg.m_notify.interrupts & kbd_set) {
            kbc_ih(); // 1. O handler lê o byte do teclado
            last=get_counter();

            if (error) continue;

            uint8_t scancode = get_scancode();
            bytes[size]=scancode;
            size++;
            
            if (scancode==0xE0) continue;
      
            //chegando aqui então o bytes já está prenchido quer por 1 ou 2 bytes

            if (scancode&BREAKCODE) make=false;
            else make=true;
            
            kbd_print_scancode(make, size, bytes);

            if (scancode == 0x81){//se for ESC
              running = false;
            } 
            
            size=0;
          }
          break;
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int()!=0) return 1;
  if (timer_unsubscribe_int()!=0) return 1;

  return 0;
}



