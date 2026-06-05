#include "fw/drivers/mouse.h"
#include "fw/common/utils.h"
#include "fw/hw/i8042.h"

#define MOUSE_MAX_TRIES 5

static int packet_size = 3;

static int hook_id = 2;
static int packet_index = 0;
static bool packet_ready = false;
static bool error = false;
static uint8_t packet_bytes[4];


uint8_t *get_packet() {
  return packet_bytes;
}

bool is_packet_ready() {
  return packet_ready;
}

static void mouse_parse_packet(mouse_packet *pp) {
  pp->bytes[0] = packet_bytes[0];
  pp->bytes[1] = packet_bytes[1];
  pp->bytes[2] = packet_bytes[2];
  pp->bytes[3] = (packet_size == 4) ? packet_bytes[3] : 0;

  pp->lb = packet_bytes[0] & BIT(0);
  pp->rb = packet_bytes[0] & BIT(1);
  pp->mb = packet_bytes[0] & BIT(2);

  pp->x_ov = packet_bytes[0] & BIT(6);
  pp->y_ov = packet_bytes[0] & BIT(7);

  // Sign extension
  pp->delta_x = (packet_bytes[0] & BIT(4)) ? (0xFF00 | packet_bytes[1]) : packet_bytes[1];
  pp->delta_y = (packet_bytes[0] & BIT(5)) ? (0xFF00 | packet_bytes[2]) : packet_bytes[2];
  pp->delta_z = (packet_size == 4) ? (int8_t)packet_bytes[3] : 0;
}

bool get_error() {
  return error;
}

int build_packet(mouse_packet *pp) {
  if (!pp) return fail(ERR_MOUSE, "build_packet: NULL packet pointer");
  if (!packet_ready) return fail(ERR_MOUSE, "build_packet: packet not ready");

  mouse_parse_packet(pp);
  packet_ready = false;
  return 0;
}

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = hook_id;

  if (sys_irqsetpolicy(KBD_AUX_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    return fail(ERR_MOUSE, "mouse_subscribe_int: sys_irqsetpolicy failed");
  }

  return 0;
}


int mouse_unsubscribe_int() {

  if (sys_irqrmpolicy(&hook_id) != OK) {
    return fail(ERR_MOUSE, "mouse_unsubscribe_int: sys_irqrmpolicy failed");
  }

  return 0;
}

void (mouse_ih)() {

  uint8_t status;
  uint8_t data;
  error = false;

  // Read status register
  if (util_sys_inb(KBC_STATUS_REG, &status) != OK) {
    error = true;
    fail(ERR_MOUSE, "mouse_ih: failed to read status register");
    return;
  }
  
  // Check if output buffer is full — retry briefly if not yet ready
  int obf_tries = 10;
  while (!(status & KBC_ST_OBF) && obf_tries-- > 0) {
    tickdelay(micros_to_ticks(20000));
    if (util_sys_inb(KBC_STATUS_REG, &status) != OK) {
      error = true;
      fail(ERR_MOUSE, "mouse_ih: failed to re-read status register");
      return;
    }
  }
  if (!(status & KBC_ST_OBF)) {
    error = true;
    fail(ERR_MOUSE, "mouse_ih: output buffer not full");
    return;
  }
  
  // Check for errors
  if (status & (KBC_PAR_ERR | KBC_TOUT_ERR)) {
    error = true;
    fail(ERR_MOUSE, "mouse_ih: parity or timeout error");
    return;
  }

  // Ignore non-mouse data
  if (!(status & KBC_MOUSE_DATA)) {
    return;
  }
  
  // Read byte
  if (util_sys_inb(KBC_DATA_REG, &data) != OK) {
    error = true;
    fail(ERR_MOUSE, "mouse_ih: failed to read data register");
    return;
  }
  
  // This makes sure the first packet_bytes is the right one
  // bit 3 is always 1
  if (packet_index == 0 && !(data & BIT(3))) return;

  packet_bytes[packet_index] = data;
  packet_index++;

  if (packet_index == packet_size) {
    packet_ready = true;
    packet_index = 0;
  } else {
    packet_ready = false;
  }

}

int (mouse_write_command)(uint8_t cmd) {
  uint8_t response;
  int attempts = MOUSE_MAX_TRIES;

  while (attempts--) {

    // Tell KBC we are sending to mouse
    if (kbc_wait_input_empty() != OK)
      return fail(ERR_MOUSE, "mouse_write_command: input buffer not empty (write cmd)");
    if (sys_outb(KBC_STATUS_REG, MOUSE_COMMAND) != OK)
      return fail(ERR_MOUSE, "mouse_write_command: failed to send mouse command");

    // Send command
    if (kbc_wait_input_empty() != OK)
      return fail(ERR_MOUSE, "mouse_write_command: input buffer not empty (write data)");
    if (sys_outb(KBC_DATA_REG, cmd) != OK)
      return fail(ERR_MOUSE, "mouse_write_command: failed to write command");

    // Read response
    if (kbc_wait_output_full(&response) != OK)
      return fail(ERR_MOUSE, "mouse_write_command: failed to read response");

    if (response == SUCCESS) return 0;
    if (response != FAILURE)
      return fail(ERR_MOUSE, "mouse_write_command: unexpected response");
    // if FAILURE (NACK) -> retry
  }

  return fail(ERR_MOUSE, "mouse_write_command: retries exceeded");
}

int mouse_enable_wheel_mode() {
  int res;

  // IntelliMouse detection sequence
  uint8_t seq[3] = {200, 100, 80};

  for (int i = 0; i < 3; i++) {
    res = mouse_write_command(SET_SAMPLE_RATE);
    if (res != OK) return res;

    res = mouse_write_command(seq[i]);
    if (res != OK) return res;
  }

  // Get device ID
  res = mouse_write_command(GET_DEVICE_ID);
  if (res != OK) return res;

  // id meaning: 
  // 0 - PS/2
  // 3 - IntelliMouse (scroll wheel)
  // 4 - IntelliMouse + extras
  uint8_t id;
  if (kbc_wait_output_full(&id) != OK)
    return fail(ERR_MOUSE, "mouse_enable_wheel_mode: failed to read device id");

  packet_size = (id >= 3) ? 4 : 3;

  packet_index = 0;
  packet_ready = false;

  // Re-enable reporting
  res = mouse_write_command(ENABLE_DR);
  if (res != OK) return res;

  return 0;
}

int my_mouse_enable_data_reporting() {
  return mouse_write_command(ENABLE_DR);
}

int my_mouse_disable_data_reporting() {
  return mouse_write_command(DISABLE_DR);
}

void my_mouse_print_packet(mouse_packet*pp) {
  printf("B1=0x%02X ", pp->bytes[0]);
  printf("B2=0x%02X ", pp->bytes[1]);
  printf("B3=0x%02X ", pp->bytes[2]);
  if (packet_size == 4) printf("B4=0x%02X ", pp->bytes[3]);

  printf("LB:%d RB:%d MB:%d ", pp->lb, pp->rb, pp->mb);
  printf("XOV:%d YOV:%d ", pp->x_ov, pp->y_ov);

  printf("dx:%d dy:%d ", pp->delta_x, pp->delta_y);

  // Scroll wheel
  printf("dz:%d", pp->delta_z);

  printf("\n");
}
