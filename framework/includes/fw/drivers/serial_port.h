#pragma once

#include <lcom/lcf.h>
#include <stdint.h>

int (serial_init)();
void (serial_ih)();

int (serial_subscribe_int)(uint8_t *bit_no);
int (serial_unsubscribe_int)();

bool (serial_read_char)(uint8_t *data);
int (serial_write_char)(uint8_t data);
