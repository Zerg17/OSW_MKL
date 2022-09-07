#include "ws2812.h"
#include "system.h"

// 0.250 19
// 0.870 38
// 1.25

rgb led[LED_NUM];
uint8_t ws2812Buf[LED_NUM*24+1];

void ws2812Init(void){
    DMA1_Channel5->CPAR = (uint32_t) (&(TIM1->CCR2));
    
    DMA1_Channel5->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_PSIZE_0;
    

    TIM1->CCMR1 = (0b110 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
    TIM1->CCER = TIM_CCER_CC2E;
    TIM1->PSC = 0;
    TIM1->ARR = 59; // 59
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->DIER |= TIM_DIER_UDE;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void ws2812Write(void){
    for(int i = 0; i<LED_NUM*24; i++){
        ws2812Buf[i]=((((uint8_t*)led)[i/8]<<i%8)&0x80)?47:17;//47 17
    }
    DMA1_Channel5->CCR &=~DMA_CCR_EN;
    DMA1_Channel5->CMAR = (uint32_t)ws2812Buf;
    DMA1_Channel5->CNDTR = sizeof(ws2812Buf);
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

rgb setHSVfast(uint8_t h, uint8_t s, uint8_t v) {      
    rgb temp;  
    uint8_t value = ((24 * h / 17) / 60) % 6;
    uint8_t vmin = (long)v - v * s / 255;
    uint8_t a = (long)v * s / 255 * (h * 24 / 17 % 60) / 60;
    uint8_t vinc = vmin + a;
    uint8_t vdec = v - a;
    switch (value) {
        case 0: temp.r = v; temp.g = vinc; temp.b = vmin; break;
        case 1: temp.r = vdec; temp.g = v; temp.b = vmin; break;
        case 2: temp.r = vmin; temp.g = v; temp.b = vinc; break;
        case 3: temp.r = vmin; temp.g = vdec; temp.b = v; break;
        case 4: temp.r = vinc; temp.g = vmin; temp.b = v; break;
        case 5: temp.r = v; temp.g = vmin; temp.b = vdec; break;
    }
    return temp;
}

rgb mHSV(uint8_t h, uint8_t s, uint8_t v) {	
    float r=0, g=0, b=0;
    
    float H = h / 255.0;
    float S = s / 255.0;
    float V = v / 255.0;
    
    int i = H * 6;
    float f = H * 6 - i;
    float p = V * (1 - S);
    float q = V * (1 - f * S);
    float t = V * (1 - (1 - f) * S);
    
    switch (i % 6) {
    case 0: r = V, g = t, b = p; break;
    case 1: r = q, g = V, b = p; break;
    case 2: r = p, g = V, b = t; break;
    case 3: r = p, g = q, b = V; break;
    case 4: r = t, g = p, b = V; break;
    case 5: r = V, g = p, b = q; break;	
    }
    r *= 255.0;
    g *= 255.0;
    b *= 255.0;

    rgb temp;
    temp.r=(uint8_t)r;
    temp.g=(uint8_t)g;
    temp.b=(uint8_t)b;

    return temp;
}