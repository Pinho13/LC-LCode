#include "mouse.h"

#include <stdbool.h>

static int hook_id = 1;
static int packet_index = 0;
static bool packet_ready = false;
static uint8_t packet[MOUSE_PACKET_SIZE];

uint8_t *get_packet() {
  return packet;
}

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = hook_id;

  if (sys_irqsetpolicy(KBD_AUX_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    return 1;
  }

  return 0;
}


int mouse_unsubscribe_int() {

  if (sys_irqrmpolicy(&hook_id) != OK) {
    return 1;
  }

  return 0;
}

void (mouse_ih)() {

  uint8_t status;
  uint8_t data;

  // Read status register
  if (util_sys_inb(KBC_STATUS_REG, &status) != OK)
    return;
  
  // Check if output buffer is full
  if (!(status & OBF))
    return;
  
  // Check for errors
  if (status & (PAR_ERR | TO_ERR))
    return;
  
  // Read byte
  if (util_sys_inb(KBC_OUT_BUF, &data) != OK)
    return;
  
  // This makes sure the first packet is the right one
  // bit 3 is always 1
  if (packet_index == 0 && !(data & BIT(3))) return;

  packet[packet_index] = data;
  packet_index++;

  if (packet_index == MOUSE_PACKET_SIZE) {
    packet_ready = true;
    packet_index = 0;
  } else {
    packet_ready = false;
  }

}

// Wait until input buffer is empty
int (kbc_wait_input_empty)() {
    uint8_t status;
    int tries = 10;

    while (tries--) {
        util_sys_inb(KBC_STATUS_REG, &status);
        if ((status & IBF) == 0) return 0;
        tickdelay(micros_to_ticks(20000));
    }
    return 1;
}

// Wait for output buffer full
int (kbc_wait_output_full)(uint8_t *data) {
    uint8_t status;
    int tries = 10;

    while (tries--) {
        util_sys_inb(KBC_STATUS_REG, &status);
        if (status & OBF) {
            util_sys_inb(KBC_OUT_BUF, data);
            return 0;
        }
        tickdelay(micros_to_ticks(20000));
    }
    return 1;
}

int (mouse_write_command)(uint8_t cmd) {
  uint8_t response;
  int attempts = MAX_RETRIES;

  while (attempts--) {

    // Tell KBC we are sending to mouse
    if (kbc_wait_input_empty() != 0) return 1;
    if (sys_outb(KBC_CMD_REG, WRITE_MOUSE) != OK) return 1;

    // Send command
    if (kbc_wait_input_empty() != 0) return 1;
    if (sys_outb(KBC_ARG_REG, cmd) != OK) return 1;

    // Read response
    if (kbc_wait_output_full(&response) != 0) return 1;

    if (response == ACK) return 0;
    if (response == ERROR) return 1;
    // if NACK → retry
  }

  return 1;
}

int (mouse_enable_data_reporting)() {
    return mouse_write_command(MOUSE_ENABLE);
}

int (mouse_disable_data_reporting)() {
    return mouse_write_command(MOUSE_DISABLE);
}