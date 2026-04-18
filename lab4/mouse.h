#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>

#include <stdbool.h>

// REGISTERS
#define KBC_STATUS_REG 0x64
#define KBC_OUT_BUF    0x60
#define KBC_CMD_REG    0x64
#define KBC_ARG_REG    0x60

// STATUS
#define OBF     BIT(0)
#define IBF     BIT(0)
#define PAR_ERR BIT(7)
#define TO_ERR  BIT(6)

// MOUSE RESPONSES
#define ACK   0xFA
#define NACK  0xFE
#define ERROR 0xFC

// PACKET STRUCTURE
#define MOUSE_PACKET_SIZE 3

// MOUSE COMMANDS
#define MOUSE_ENABLE  0xF4
#define MOUSE_DISABLE 0xF5

// MOUSE NUMBER OF ATTEMPTS
#define MAX_RETRIES 5

// ALLOWS SENDING COMMANDS TO THE MOUSE
#define WRITE_MOUSE 0xD4

uint8_t *get_packet();
bool is_packet_ready();

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();

void mouse_parse_packet(struct packet *pp);

void (mouse_ih)();

int my_mouse_enable_data_reporting();
int my_mouse_disable_data_reporting();

#endif
