#pragma once

#include "fw/drivers/timer.h"
#include "fw/drivers/keyboard.h"
#include "fw/drivers/mouse.h"

int subscribe_interrupts();
int unsubscribe_interrupts();

void timer_handler();
void keyboard_handler();
void mouse_handler();

void interrupts_handler(uint32_t irq_mask);
