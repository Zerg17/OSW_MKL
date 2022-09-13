#pragma once

#include "stdint.h"

#define LED_NUM 12

typedef enum{
    power_led,
    xor_led,
    i1_led,
    gt_led,
    o2_led,
    osc_led,
    and_led,
    not_led,
    o1_led,
    or_led,
    i2_led,
    dtr_led
} led_e;

#pragma pack(push, 1)
typedef union {
    struct{
        uint8_t g;
        uint8_t r;
        uint8_t b;
    };
    uint32_t c:24;
} rgb;
#pragma pack(pop)

extern rgb led[LED_NUM];

void ws2812Init(void);
void ws2812Write(void);
rgb setHSVfast(uint8_t h, uint8_t s, uint8_t v);
rgb mHSV(uint8_t h, uint8_t s, uint8_t v);