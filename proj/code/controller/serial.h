#pragma once

#include <lcom/lcf.h>

/**
 * @file serial.h
 * @brief Serial port protocol: packet framing, command codes, and incoming data processing
 */

/** @brief Start byte that marks the beginning of every serial packet */
#define PKT_START_BYTE 0xFE

/** @brief A received serial packet with command code and raw payload */
typedef struct {
  uint8_t cmd;
  uint8_t payload_buf[256];
  uint8_t payload_len;
} SerialEvent;

/** @brief Serial command codes and their payload format */
typedef enum {
  CMD_INSERT_CHAR = 0x01,  /**< Payload: 1 byte (character) */
  CMD_DELETE_CHAR = 0x02,  /**< Payload: 0 bytes */
  CMD_MOVE_CURSOR = 0x03,  /**< Payload: 4 bytes (row and column as two uint16_t) */
  CMD_FILE_START  = 0x10,  /**< Payload: filename string + 2 bytes (line count) */
  CMD_FILE_LINE   = 0x11,  /**< Payload: line content string */
} SerialCommand;

/**
 * @brief Reads available bytes from the serial port and dispatches complete packets
 *
 * Should be called from the serial interrupt handler
 */
void serial_process();
