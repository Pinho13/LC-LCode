#include <lcom/lcf.h>

#include "mouse.h"
#include "kbc.h"

static int hook_id = MOUSE_IRQ;

static uint8_t packet_buf[PACKET_SIZE];
static uint8_t byte_idx = 0;
static bool ih_error = true;
static bool pkt_ready = false;

bool mouse_packet_ready() {
  return pkt_ready;
}

struct packet mouse_get_packet() {
  struct packet pp;

  pp.bytes[0] = packet_buf[0];
  pp.bytes[1] = packet_buf[1];
  pp.bytes[2] = packet_buf[2];

  pp.lb = (packet_buf[0] & MOUSE_LB) != 0;
  pp.rb = (packet_buf[0] & MOUSE_RB) != 0;
  pp.mb = (packet_buf[0] & MOUSE_MB) != 0;

  pp.x_ov = (packet_buf[0] & MOUSE_X_OVF) != 0;
  pp.y_ov = (packet_buf[0] & MOUSE_Y_OVF) != 0;

  pp.delta_x = (int16_t) packet_buf[1];
  if (packet_buf[0] & MOUSE_X_SIGN) {
    pp.delta_x |= 0xFF00;
  }

  pp.delta_y = (int16_t) packet_buf[2];
  if (packet_buf[0] & MOUSE_Y_SIGN) {
    pp.delta_y |= 0xFF00;
  }

  return pp;
}

int mouse_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    return 1;
  }
  *bit_no = (uint8_t) hook_id;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0) {
    return 1;
  }
  return 0;
}

int mouse_unsubscribe_int() {
  return sys_irqrmpolicy(&hook_id);
}

int mouse_send_cmd(uint8_t cmd) {
  uint8_t ack;
  for (int i = 0; i < KBC_RETRIES; i++) {
    if (kbc_issue_cmd(KBC_WRITE_AUX) != 0) {
      return 1;
    }
    if (kbc_write_arg(cmd) != 0) {
      return 1;
    }
    if (kbc_read_response(&ack) != 0) {
      return 1;
    }
    if (ack == MOUSE_ACK) {
      return 0;
    }
  }
  return 1;
}

int mouse_disable_data_reporting() {
  return mouse_send_cmd(MOUSE_DISABLE_DR);
}

void (mouse_ih)() {
  ih_error = true;
  pkt_ready = false;

  uint8_t st;
  if (util_sys_inb(KBC_STATUS_REG, &st) != 0) {
    return;
  }

  if (!(st & KBC_ST_OBF)) {
    return;
  }

  uint8_t byte;
  if (util_sys_inb(KBC_OUTPUT_BUF, &byte) != 0) {
    return;
  }

  if (!(st & KBC_ST_AUX)) {
    return;
  }

  if (st & (KBC_ST_TO_ERR | KBC_ST_PAR_ERR)) {
    byte_idx = 0;
    return;
  }

  ih_error = false;

  if (byte_idx == 0 && !(byte & MOUSE_SYNC_BIT)) {
    return;
  }

  packet_buf[byte_idx++] = byte;

  if (byte_idx == PACKET_SIZE) {
    byte_idx = 0;
    pkt_ready = true;
  }
}
