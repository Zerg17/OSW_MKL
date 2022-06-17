#include "system.h"

volatile uint8_t* sendPoint;
volatile uint8_t sendCnt;

volatile uint8_t bitShift;
volatile uint8_t byteShift;

static const struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} oswPins[4] = {
    {GPIOA, 4},
    {GPIOA, 6},
    {GPIOA, 7},
    {GPIOB, 1},
};

volatile GPIO_TypeDef *transmitPort;
volatile uint8_t transmitPin;

void oswSend(uint8_t* buf, uint8_t len, uint8_t pin) {
    while(sendCnt);
    transmitPort = oswPins[pin].port;
    transmitPin = oswPins[pin].pin;
    sendPoint=buf;
    sendCnt=len;
}

void TIM17_IRQHandler(){
    GPIOA->BSRR = GPIO_BSRR_BS_9;
    TIM17->SR = 0;
    if(sendCnt){
        if(!bitShift){ 
            transmitPort->BRR = transmitPin;
            bitShift=1;
            TIM17->CNT=(((*sendPoint)>>byteShift)&1)?(65535-200):(65535-400);
        }else{
            transmitPort->BSRR = transmitPin;
            bitShift=0;
            TIM17->CNT=65535-200;
            byteShift++;
        }
        if(byteShift>7){
            byteShift=0;
            sendPoint++;
            sendCnt--;
        }
    }
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}