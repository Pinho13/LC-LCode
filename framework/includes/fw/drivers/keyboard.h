#pragma once

#include <lcom/lcf.h>
#include <stdint.h>
#include "fw/hw/i8042.h"

typedef struct {
  bool two_byte;
  bool make;
  unsigned char size;
  unsigned char bytes[2];
} packet_scancode;

char scancode_to_char(unsigned char scancode);
const char *scancode_to_name(uint8_t scancode, bool two_byte);
void print_scancode_value();

void set_scancode(uint8_t code);
uint8_t get_scancode();
bool get_error_keyboard();

int build_scancode(packet_scancode *ps);
void (keyboard_ih)();

int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
int (keyboard_print_scancode)(packet_scancode ps);
