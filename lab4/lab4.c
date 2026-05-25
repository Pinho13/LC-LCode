#include <lcom/lcf.h>
#include <lcom/lab4.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "mouse.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  if (lcf_start(argc, argv)) {
    return 1;
  }

  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;
  if (mouse_subscribe_int(&bit_no) != 0) {
    return 1;
  }
  uint32_t irq_set = BIT(bit_no);

  if (mouse_enable_data_reporting() != 0) {
    mouse_unsubscribe_int();
    return 1;
  }

  uint32_t packets_read = 0;
  int ipc_status;
  message msg;

  while (packets_read < cnt) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if (mouse_packet_ready()) {
              struct packet pp = mouse_get_packet();
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

  if (mouse_disable_data_reporting() != 0) {
    mouse_unsubscribe_int();
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    return 1;
  }
  uint32_t mouse_irq_set = BIT(mouse_bit_no);

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) != 0) {
    mouse_unsubscribe_int();
    return 1;
  }
  uint32_t timer_irq_set = BIT(timer_bit_no);

  if (mouse_send_cmd(MOUSE_ENABLE_DR) != 0) {
    timer_unsubscribe_int();
    mouse_unsubscribe_int();
    return 1;
  }

  uint32_t hz = sys_hz();
  uint32_t max_idle_ticks = (uint32_t) idle_time * hz;
  uint32_t idle_ticks = 0;

  bool done = false;
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
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();

            if (mouse_packet_ready()) {
              struct packet pp = mouse_get_packet();
              mouse_print_packet(&pp);
              idle_ticks = 0;
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            idle_ticks++;

            if (idle_ticks >= max_idle_ticks) {
              done = true;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (mouse_disable_data_reporting() != 0) {
    timer_unsubscribe_int();
    mouse_unsubscribe_int();
    return 1;
  }

  if (timer_unsubscribe_int() != 0) {
    mouse_unsubscribe_int();
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}
