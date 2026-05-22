#pragma once

#include <lcom/lcf.h>
#include <stdint.h>
#include "fw/hw/i8042.h"

#define MOUSE_PACKET_SIZE 3

uint8_t *get_packet();
bool is_packet_ready();

void (mouse_ih)(void);
bool get_error();
int build_packet(struct packet *pp);

int my_mouse_disable_data_reporting();
int my_mouse_enable_data_reporting();

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();

int mouse_write_command(uint8_t cmd);

