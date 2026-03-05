#include "bitwise.h"
#include <stdarg.h>

uint8_t clear(uint8_t msk, int pos) { return msk & ~(1 << pos); }

uint8_t set(uint8_t msk, int pos) { return msk | (1 << pos); }

bool is_set(uint8_t msk, int pos) {
  uint8_t aux = (1 << pos);
  aux = msk & aux;
  if (aux) {
    return true;
  }
  return false;
}

uint8_t
lsb(uint16_t wide_msk) {    // podia apenas dar cast return (uint8_t) wide_msk;
  uint8_t aux = 0b11111111; // ou 0xFF
  aux = aux & wide_msk;
  return aux; // aqui faz o cast implicitamente para uint8_t
}

uint8_t msb(uint16_t wide_msk) {
  wide_msk = wide_msk >> 8;
  return (uint8_t)wide_msk;
}

uint8_t mask(int pos, ...) {
  if (pos==MSK_END) return 0x00;
  va_list args;
  uint8_t aux = 0x00;
  aux=set(aux,pos);
  va_start(args,pos);//prepara a lista para ler argumentos após pos
  int next = va_arg(args,int);//extrai um argumento do tipo int
  while (next!=MSK_END){
    aux=set(aux,next);
    next = va_arg(args,int);
  }
  return aux;
}
