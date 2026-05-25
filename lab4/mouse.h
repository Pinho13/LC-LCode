#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>

#define MOUSE_IRQ        12

#define KBC_WRITE_AUX    0xD4

#define MOUSE_ENABLE_DR  0xF4
#define MOUSE_DISABLE_DR 0xF5

#define MOUSE_ACK        0xFA
#define MOUSE_NACK       0xFE
#define MOUSE_ERROR      0xFC

#define MOUSE_LB         BIT(0)
#define MOUSE_RB         BIT(1)
#define MOUSE_MB         BIT(2)
#define MOUSE_SYNC_BIT   BIT(3)
#define MOUSE_X_SIGN     BIT(4)
#define MOUSE_Y_SIGN     BIT(5)
#define MOUSE_X_OVF      BIT(6)
#define MOUSE_Y_OVF      BIT(7)

#define PACKET_SIZE      3

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();

void (mouse_ih)();
bool mouse_packet_ready();
struct packet mouse_get_packet();
int mouse_send_cmd(uint8_t cmd);
int mouse_disable_data_reporting();

#endif /* _LCOM_MOUSE_H_ */
