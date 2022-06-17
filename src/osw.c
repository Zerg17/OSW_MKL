#include "system.h"

volatile uint8_t* sendPoint;
volatile uint8_t sendCnt;

volatile uint8_t byteShift;

volatile uint8_t frameStarted, receivedByte, tLast;



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
    byteShift = 0xFF;
    sendPoint=buf;
    sendCnt=len;
}

uint8_t byteToSend;
void oswSendByte(uint8_t payload, uint8_t pin) {
    byteToSend = payload;
    oswSend(&byteToSend, 1, pin);
}

void TIM17_IRQHandler() {
    GPIOA->BSRR = GPIO_BSRR_BS_9;  // interrupt enter notification to LA
    TIM17->SR = 0;
    if (sendCnt) {
        if (byteShift & 1)
           transmitPort->BRR = transmitPin;
        else
           transmitPort->BSRR = transmitPin;
        TIM17->CNT = (((*sendPoint) >> byteShift) & 1) ? (65535 - 200) : (65535 - 400);
        byteShift++;

        if (byteShift > 7) {
            byteShift = 0;
            sendPoint++;
            sendCnt--;
        }
    } else
        transmitPort->BSRR = transmitPin;
    frameStarted = 0;
    GPIOA->BSRR = GPIO_BSRR_BR_9;  // interrupt exit notification to LA
}

void EXTI0_1_IRQHandler() {
    GPIOA->BSRR = GPIO_BSRR_BS_9;
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR |= EXTI_PR_PR1;
    }
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}

void EXTI4_15_IRQHandler() {
    GPIOA->BSRR = GPIO_BSRR_BS_9;
    uint16_t t = TIM17->CNT;
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR |= EXTI_PR_PR4;
    }
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR |= EXTI_PR_PR6;
    }
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR |= EXTI_PR_PR7;
        if (frameStarted) {
            byteShift--;
            receivedByte |= ((t - tLast) < 300) << byteShift;
            tLast = t;
            if (!byteShift) {
                uartWrite(receivedByte);
                byteShift = 8;
                receivedByte = 0;
            }
        } else {
            frameStarted = 1;
            byteShift = 9;
        }
    }
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}