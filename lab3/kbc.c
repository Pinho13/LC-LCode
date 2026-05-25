#include <lcom/lcf.h>
#include "kbc.h"

static int hook_id = KBD_IRQ;
static uint8_t scancode_byte = 0;
static bool ih_error = true;

uint8_t kbc_get_scancode() {
  return scancode_byte;
}

bool kbc_get_error() {
  return ih_error;
}

int kbd_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    return 1;
  }
  *bit_no = (uint8_t) hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0) {
    return 1;
  }
  return 0;
}

int kbd_unsubscribe_int() {
  return sys_irqrmpolicy(&hook_id);
}

int kbc_issue_cmd(uint8_t cmd) {
  uint8_t st;
  for (int i = 0; i < KBC_RETRIES; i++) {
    if (util_sys_inb(KBC_STATUS_REG, &st) != 0) return 1;
    if (!(st & KBC_ST_IBF)) {
      return sys_outb(KBC_CMD_REG, cmd);
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int kbc_write_arg(uint8_t arg) {
  uint8_t st;
  for (int i = 0; i < KBC_RETRIES; i++) {
    if (util_sys_inb(KBC_STATUS_REG, &st) != 0) return 1;
    if (!(st & KBC_ST_IBF)) {
      return sys_outb(KBC_INPUT_BUF, arg);
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int kbc_read_response(uint8_t *resp) {
  if (resp == NULL) return 1;
  uint8_t st;
  for (int i = 0; i < KBC_RETRIES; i++) {
    if (util_sys_inb(KBC_STATUS_REG, &st) != 0) return 1;
    if (st & KBC_ST_OBF) {
      if (util_sys_inb(KBC_OUTPUT_BUF, resp) != 0) return 1;
      if (st & (KBC_ST_TO_ERR | KBC_ST_PAR_ERR)) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int kbc_enable_int() {
  uint8_t cmd_byte;
  if (kbc_issue_cmd(KBC_READ_CMD_BYTE) != 0) return 1;
  if (kbc_read_response(&cmd_byte) != 0) return 1;
  cmd_byte |= KBC_CMD_INT;
  if (kbc_issue_cmd(KBC_WRITE_CMD_BYTE) != 0) return 1;
  return kbc_write_arg(cmd_byte);
}

void (kbc_ih)() {
  ih_error = true;

  uint8_t st;
  if (util_sys_inb(KBC_STATUS_REG, &st) != 0) {
    return;
  }

  if (!(st & KBC_ST_OBF)) {
    return;
  }

  // Always drain the buffer, even on error
  if (util_sys_inb(KBC_OUTPUT_BUF, &scancode_byte) != 0) {
    return;
  }

  if (st & KBC_ST_AUX) {
    return;
  }

  if (st & (KBC_ST_TO_ERR | KBC_ST_PAR_ERR)) {
    return;
  }

  ih_error = false;
}
