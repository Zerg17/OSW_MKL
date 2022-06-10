#pragma once

#include "stm32f0xx.h"
#include "xprintf.h"

#define BAUD 500000

void sysInit();
void uartWrite(uint8_t d);