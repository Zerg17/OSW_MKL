#pragma once
#include "stdint.h"

void oswSend(uint8_t* buf, uint8_t len, uint8_t pin);
void oswSendByte(uint8_t payload, uint8_t pin);
