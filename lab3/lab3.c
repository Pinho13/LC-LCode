#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "keyboard.h"

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

  struct packet_scancode ps;

  bool running = true;
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_set) {
            
            kbc_ih(); 

            if (get_error_keyboard()) continue;

            uint8_t scancode = get_scancode();
            
            if (build_scancode(&ps)==1) continue;
            
            kbd_print_scancode(ps.make, ps.size, ps.bytes);

            if (scancode == 0x81){
              running = false;
            } 

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
  struct packet_scancode ps;
  bool running=true;

  while (running){
    kbc_ih(); 

    if (get_error_keyboard()){
      tickdelay(micros_to_ticks(20000));
      continue;
    } 

    uint8_t scancode = get_scancode();
    
    if (build_scancode(&ps)==1){
      tickdelay(micros_to_ticks(20000));
      continue;
    } 
            
    kbd_print_scancode(ps.make, ps.size, ps.bytes);

    if (scancode == 0x81){
      running = false;
    } 
  
    tickdelay(micros_to_ticks(20000));
  }
  
  //voltar a logar as interrupções
  if (kbc_enable_interrupts()!=0) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd = 1, bit_no_timer = 0;
  uint32_t kbd_set = BIT(bit_no_kbd);
  uint32_t timer_set = BIT(bit_no_timer);
  
  if (keyboard_subscribe_int(&bit_no_kbd) != 0) return 1;
  if (timer_subscribe_int(&bit_no_timer) != 0) return 1;

  int r, ipc_status;
  message msg;
  struct packet_scancode ps;
  bool running = true;

  uint32_t last = get_counter(); 
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
      
          if (msg.m_notify.interrupts & kbd_set) {
            kbc_ih(); 
            
            if (!get_error_keyboard()) {
              last = get_counter(); 
              if (build_scancode(&ps) == 0) {
                kbd_print_scancode(ps.make, ps.size, ps.bytes);
                if (get_scancode() == 0x81) running = false;
              }
            }
          }

          if (msg.m_notify.interrupts & timer_set) {
            timer_int_handler();
            
            if (get_counter() - last >= (uint32_t)(n * 60)) {
              running = false;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  timer_unsubscribe_int();
  keyboard_unsubscribe_int();

  return 0;
}



