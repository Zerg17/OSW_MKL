#pragma once
#include "stdint.h"

#define LOGIC_0_PERIOD (65535 - 400)
#define LOGIC_1_PERIOD (65535 - 200)


void oswSend(uint8_t* buf, uint8_t len, uint8_t pin);
void oswSendByte(uint8_t payload, uint8_t pin);
