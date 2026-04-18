// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"

// Any header files included below this line should have been created by you

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
  uint8_t bit_no;
  int ipc_status;
  message msg;
  int r;

  uint32_t packets_read = 0;

  if (mouse_subscribe_int(&bit_no) != OK) return 1;
  if (my_mouse_enable_data_reporting() != OK) return 1;

  uint8_t irq_set = BIT(bit_no);

  while (packets_read < cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (is_packet_ready()) {
              struct packet pp;
              mouse_parse_packet(&pp);
              mouse_print_packet(&pp);

              packets_read++;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (my_mouse_disable_data_reporting() != OK) return 1;
  if (mouse_unsubscribe_int() != OK) return 1;

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint32_t timer_counter = 0;

  uint8_t mouse_bit_no;
  uint8_t timer_bit_no;
  int ipc_status;
  message msg;
  int r;

  uint32_t packets_read = 0;

  if (mouse_subscribe_int(&mouse_bit_no) != OK) return 1;
  if (my_mouse_enable_data_reporting() != OK) return 1;

  if (timer_subscribe_int(&timer_bit_no) != OK) return 1;

  uint8_t mouse_irq_set = BIT(mouse_bit_no);
  uint8_t timer_irq_set = BIT(timer_bit_no);


  while (1) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();


            if (is_packet_ready()) {
              struct packet pp;
              mouse_parse_packet(&pp);
              mouse_print_packet(&pp);

              packets_read++;
              
              timer_counter = 0;
            }
          }

          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();

            timer_counter++;

            if (timer_counter >= idle_time * 60) {
              if (timer_unsubscribe_int() != OK) return 1;
              if (my_mouse_disable_data_reporting() != OK) return 1;
              if (mouse_unsubscribe_int() != OK) return 1;
              return 0;
            }

          }
          break;
        default:
          break;
      }
    }
  }

  if (timer_unsubscribe_int() != OK) return 1;
  if (my_mouse_disable_data_reporting() != OK) return 1;
  if (mouse_unsubscribe_int() != OK) return 1;

  return 0;
}
