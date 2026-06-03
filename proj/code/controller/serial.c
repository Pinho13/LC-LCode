#include "controller/serial.h"
#include "fw/drivers/serial_port.h"
#include "model/editor.h"
#include "render_flag.h"


static void serial_handle_byte(uint8_t byte) {
  //TODO
}

void serial_process() {
  serial_ih(); 

  uint8_t data;
  while (serial_read_char(&data)) {
    serial_handle_byte(data);
  }
}
