#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"

void set_scancode(uint8_t code);
uint8_t get_scancode();
void (kbc_ih)();
int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
