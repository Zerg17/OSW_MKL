#pragma once

#include "stdint.h"

#define LED_NUM 12

typedef struct {
    uint8_t g;
    uint8_t r;
    uint8_t b;
} rgb;

extern rgb led[LED_NUM];

void ws2812Init(void);
void ws2812Write(void);
rgb setHSVfast(uint8_t h, uint8_t s, uint8_t v);
rgb mHSV(uint8_t h, uint8_t s, uint8_t v);