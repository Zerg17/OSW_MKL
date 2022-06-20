#pragma once
#include "stdint.h"

#define LOGIC_0_PERIOD (65535 - 8)
#define LOGIC_1_PERIOD (65535 - 4)
#define LOGIC_THRESHOLD (0x8)


void oswSend(uint8_t* buf, uint8_t len, uint8_t pin);
void oswSendByte(uint8_t payload, uint8_t pin);
void oswReceiverReset(void);
