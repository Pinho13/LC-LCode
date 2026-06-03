#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

/* I/O ports */
#define KBC_STATUS_REG 0x64
#define KBC_OUT_BUF    0x60

/* Status Register bits */
#define OBF     BIT(0)   /* Output Buffer Full: data available to read */
#define IBF     BIT(1)   /* Input Buffer Full: do NOT write yet */
#define PAR_ERR BIT(7)   /* Parity error - discard data */
#define TO_ERR  BIT(6)   /* Timeout error - discard data */

/* Scancode constants */
#define TWO_BYTE  0xE0   /* Prefix for 2-byte scancodes */
#define ESC_BREAK 0x81   /* Breakcode of ESC key */

/* KBC Command Byte commands */
#define KBC_READ_CMD  0x20
#define KBC_WRITE_CMD 0x60

/* Getter/setter for the byte read by the interrupt handler */
uint8_t get_scancode_byte(void);
void    set_scancode_byte(uint8_t byte);

int  kbd_subscribe_int(uint8_t *bit_no);
int  kbd_unsubscribe_int(void);

void (kbc_ih)(void);

#endif /* KBC_H */
