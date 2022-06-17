#pragma once

#include "osw.h"
#include "stm32f0xx.h"
#include "xprintf.h"

#define BAUD 2000000

void sysInit();
void uartWrite(uint8_t d);
