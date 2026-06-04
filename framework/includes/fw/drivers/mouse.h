#pragma once

#include <lcom/lcf.h>
#include <stdint.h>
#include "fw/hw/i8042.h"

#define MOUSE_PACKET_SIZE 3

typedef struct {
  uint8_t bytes[4]; // mouse packet raw bytes (4th byte for wheel mode)
  bool rb, mb, lb;  // right, middle and left mouse buttons pressed
  int16_t delta_x;  // mouse x-displacement: rightwards is positive
  int16_t delta_y;  // mouse y-displacement: upwards is positive
  int8_t delta_z;   // mouse wheel displacement: upwards is positive
  bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
} mouse_packet;

uint8_t *get_packet();
bool is_packet_ready();

void (mouse_ih)(void);
bool get_error();
int build_packet(mouse_packet *pp);

int my_mouse_disable_data_reporting();
int my_mouse_enable_data_reporting();

int mouse_enable_wheel_mode();

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();

int mouse_write_command(uint8_t cmd);

void my_mouse_print_packet(mouse_packet*pp);
