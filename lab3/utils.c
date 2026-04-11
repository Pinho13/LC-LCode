#include <lcom/lcf.h>

#include <stdint.h>

// Reads value from port
int (util_sys_inb)(int port, uint8_t *value) {
  
  uint32_t temp;

  if (sys_inb(port, &temp) != OK)
    return 1;
  
  *value = (uint8_t) temp;

  return 0;
}

// Writes value to port
int util_sys_outb(int port, uint8_t value) {
  
  if (sys_outb(port, value) != OK) {
    return 1;
  }

  return 0;
}
