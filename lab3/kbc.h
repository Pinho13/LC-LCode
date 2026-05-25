#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>

#define KBD_IRQ 1

#define KBC_OUTPUT_BUF  0x60
#define KBC_INPUT_BUF   0x60
#define KBC_STATUS_REG  0x64
#define KBC_CMD_REG     0x64

#define KBC_ST_OBF     BIT(0)
#define KBC_ST_IBF     BIT(1)
#define KBC_ST_AUX     BIT(5)
#define KBC_ST_TO_ERR  BIT(6)
#define KBC_ST_PAR_ERR BIT(7)

#define TWO_BYTE_PREFIX 0xE0
#define ESC_BREAK       0x81
#define BREAK_BIT       BIT(7)

#define KBC_READ_CMD_BYTE   0x20
#define KBC_WRITE_CMD_BYTE  0x60
#define KBC_CMD_INT         BIT(0)
#define KBC_RETRIES         10
#define DELAY_US            20000

int kbd_subscribe_int(uint8_t *bit_no);
int kbd_unsubscribe_int();
void (kbc_ih)();
uint8_t kbc_get_scancode();
bool kbc_get_error();
int kbc_issue_cmd(uint8_t cmd);
int kbc_write_arg(uint8_t arg);
int kbc_read_response(uint8_t *resp);
int kbc_enable_int();

#endif /* _LCOM_KBC_H_ */
