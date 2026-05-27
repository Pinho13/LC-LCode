#pragma once

#include "fw/drivers/timer.h"
#include "fw/drivers/keyboard.h"
#include "fw/drivers/mouse.h"

int subscribe_interrupts();
int unsubscribe_interrupts();
