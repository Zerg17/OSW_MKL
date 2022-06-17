#include "system.h"

volatile uint8_t* sendPoint;    // Pointer to the buffer to be sent
volatile uint8_t sendCnt;       // Number of bytes to be sent
volatile uint8_t byteShift;     // Number of bits to be shifted out
volatile GPIO_TypeDef *transmitPort;    // GPIO port to be used for transmission
volatile uint8_t transmitPin;           // GPIO pin to be used for transmission

volatile uint8_t frameStarted, receivedByte, tLast;

static const struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} oswPins[4] = {
    {GPIOA, 1<<4},
    {GPIOA, 1<<6},
    {GPIOA, 1<<7},
    {GPIOB, 1<<1},
};

void oswSend(uint8_t* buf, uint8_t len, uint8_t pin) {
    while(sendCnt);
    transmitPort = oswPins[pin].port;   // Set GPIO port to be used for transmission
    transmitPin = oswPins[pin].pin;     // Set GPIO pin to be used for transmission
    sendPoint=buf;                      // Set pointer to the buffer to be sent
    sendCnt=len;                        // Set number of bytes to be sent
    byteShift=0xFF;                     // Set byte shift
}

void oswSendByte(uint8_t payload, uint8_t pin) {
    oswSend(&payload, 1, pin);
}

void TIM17_IRQHandler() {
    GPIOA->BSRR = GPIO_BSRR_BS_9;   // interrupt enter notification to LA
    TIM17->SR = 0;                  // clear interrupt flag
    if (sendCnt) {                  // if there is something to send
        if (byteShift & 1) transmitPort->BRR = transmitPin; 
        else transmitPort->BSRR = transmitPin;
        TIM17->CNT = (((*sendPoint) >> byteShift) & 1) ? (65535 - 200) : (65535 - 400);
        byteShift++;
        if (byteShift > 7) {
            byteShift = 0;
            sendPoint++;
            sendCnt--;
        }
    } else transmitPort->BSRR = transmitPin;
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