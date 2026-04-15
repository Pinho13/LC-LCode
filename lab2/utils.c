#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {//o byte mais á direita
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {//o byte mais á esquerda
  val = val >> 8;
  *msb = (uint8_t) val;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {//ler do hardware
  uint32_t val32; 
  if (sys_inb(port, &val32) != 0) return 1;
  *value = (uint8_t) val32; // Cast para 8 bits e guarda no ponteiro
  return 0;
}
