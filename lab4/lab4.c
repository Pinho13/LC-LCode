// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"

uint32_t get_counter();

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  uint8_t hook_id = 12;
  uint32_t mouse_set = BIT(hook_id);
  
  if (mouse_enable_data_reporting_1()!=0) return 1;
  if (mouse_subscribe_int(&hook_id)!=0) return 1;

  int r, ipc_status;
  message msg;

  bool running = true;
  uint32_t contador = 0;

  struct packet pp;
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_set) {
            
            mouse_ih();
            
            if (get_error()) continue;

            if (build_packet(&pp)==1){
              mouse_print_packet(&pp);
              contador++;
            } 

            if (contador==cnt) running = false;
          }
          break;
        default:
          break;
      }
    }
  }

  if (mouse_unsubscribe_int()!=0) return 1;
  if (mouse_disable_data_reporting()!=0) return 1;
  
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint8_t hook_id_mouse = 12;
  uint8_t hook_id_timer = 0;

  uint32_t mouse_set = BIT(hook_id_mouse);
  uint32_t timer_set = BIT(hook_id_timer);
  
  if (mouse_enable_data_reporting_1()!=0) return 1;
  if (mouse_subscribe_int(&hook_id_mouse)!=0) return 1;
  if (timer_subscribe_int(&hook_id_timer)!=0) return 1;

  int r, ipc_status;
  message msg;

  bool running = true;
  uint32_t last = 0;
  uint32_t timer = 0;

  struct packet pp;
  
  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_set) {
            
            mouse_ih();
            
            if (get_error()) continue;

            if (build_packet(&pp)==1){
              mouse_print_packet(&pp);
              last=get_counter();
            }
            
          }
          if (msg.m_notify.interrupts & timer_set){

            timer_int_handler();

            timer=get_counter();

            if (timer-last >= (uint32_t)(idle_time)*60){
              running=false;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (mouse_unsubscribe_int()!=0) return 1;
  if (timer_unsubscribe_int()!=0) return 1;
  if (mouse_disable_data_reporting()!=0) return 1;
  
  return 0;
} 
