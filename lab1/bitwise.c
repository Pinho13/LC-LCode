#include "bitwise.h"
#include <stdarg.h>

#define TODO return 255

uint8_t clear(uint8_t msk, int pos) { 
  uint8_t ans = 00000001;
  ans = ans << pos;
  ans = ~ans;
  return msk&ans;
}

uint8_t set(uint8_t msk, int pos) {
  uint8_t ans = 00000001;
  ans = ans << pos; 
  return msk | ans; 
}

bool is_set(uint8_t msk, int pos) {
  uint8_t ans = 00000001;
  ans = ans << pos;
  ans = ans & msk; 
  return (ans > 0); 
}

uint8_t lsb(uint16_t wide_msk) {
  uint16_t mask = 0x00FF;
  return (uint8_t) (mask & wide_msk); 

}

uint8_t msb(uint16_t wide_msk) {
  uint16_t mask = 0xFF00;
  mask = mask & wide_msk;
  mask = mask >> 8; 
  return (uint8_t) (mask); 
}

uint8_t mask(int pos, ...) {
  uint8_t m = 0;
  uint8_t n; 
  va_list ap;
  va_start(ap, pos);

  int p = pos;
  while (p != MSK_END) {
    n = 1; 
    n = n << p;
    m = m | n;  
    p = va_arg(ap, int);
  }

  va_end(ap);
  return m;
}
