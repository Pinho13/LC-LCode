#pragma once

#include <lcom/lcf.h>

#define PKT_START_BYTE   0xFE

typedef enum {
  CMD_INSERT_CHAR  = 0x01,  // Payload: 1 byte
  CMD_DELETE_CHAR  = 0x02,  // Payload: 0 bytes
  CMD_MOVE_CURSOR  = 0x03,  // Payload: 1 byte (Direction)
  CMD_FILE_START   = 0x10,  // Payload: String (filename) + 2 bytes (number of lines)
  CMD_FILE_LINE    = 0x11,  // Payload: String (line content)
} SerialCommand;

void serial_process();
