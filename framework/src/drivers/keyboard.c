#include "fw/drivers/keyboard.h"
#include "fw/common/utils.h"

static bool error=false;
static int hook_id=1;
static uint8_t scancode;

void set_scancode(uint8_t code){
  scancode=code;
}
uint8_t get_scancode(){
  return scancode;
}

bool did_error_occur(){
  return error;
}


int build_scancode(packet_scancode *ps){
  if (ps->size > 2)
    return fail(ERR_KEYBOARD, "build_scancode: invalid scancode size");
  
  uint8_t byte = get_scancode();

  if (byte == TWO_BYTE) {
    ps->two_byte = true;
    ps->bytes[0] = byte;
    return 0;
  }

  if (ps->two_byte) {
    ps->bytes[1] = byte;
    ps->size = 2;
    ps->two_byte = false;
  } else {
    ps->bytes[0] = byte;
    ps->size = 1;
  }
  
  ps->make = !(byte & BIT(7));
  return 0;
}


void (keyboard_ih)() {
  uint8_t status;
  uint8_t data;

  if (util_sys_inb(KBC_STATUS_REG, &status) != OK) {
    fail(ERR_KEYBOARD, "keyboard_ih: failed to read status register");
    return;
  }

  if (!(status & KBC_ST_OBF)) {
    fail(ERR_KEYBOARD, "keyboard_ih: output buffer not full");
    return;
  }

  if (status & (KBC_PAR_ERR | KBC_TOUT_ERR)) {
    fail(ERR_KEYBOARD, "keyboard_ih: parity or timeout error");
    return;
  }

  if (util_sys_inb(KBC_DATA_REG, &data) != OK) {
    fail(ERR_KEYBOARD, "keyboard_ih: failed to read data register");
    return;
  }

  set_scancode(data);
}

int (keyboard_subscribe_int)(uint8_t *bit_no){
  *bit_no = hook_id;

  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    return fail(ERR_KEYBOARD, "keyboard_subscribe_int: sys_irqsetpolicy failed");
  }

  return 0;
}

int (keyboard_unsubscribe_int)() {

  if (sys_irqrmpolicy(&hook_id) != OK) {
    return fail(ERR_KEYBOARD, "keyboard_unsubscribe_int: sys_irqrmpolicy failed");
  }

  return 0;
}

int (keyboard_print_scancode)(packet_scancode ps) {
    if (ps.size == 0) {
        return fail(ERR_KEYBOARD, "keyboard_print_scancode: invalid scancode size");
    }

    printf("%s code: ", ps.make ? "MAKE" : "BREAK");

    for (uint8_t i = 0; i < ps.size; i++) {
        printf("0x%02X", ps.bytes[i]);

        if (i < ps.size - 1) {
            printf(" ");
        }
    }

    printf("\n");

    return 0;
}
