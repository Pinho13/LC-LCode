#include <lcom/lcf.h>

#include <stdint.h>

#include "fw/common/utils.h"

// Reads value from port
int (util_sys_inb)(int port, uint8_t *value) {
  
  uint32_t temp;

  if (sys_inb(port, &temp) != OK)
    return 1;
  
  *value = (uint8_t) temp;

  return 0;
}

// Prints message before fail
int fail(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  return 1;
}

