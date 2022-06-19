#include "system.h"

volatile uint8_t* sendPoint;          // Pointer to the buffer to be sent
volatile uint8_t sendCnt;             // Number of bytes to be sent
volatile uint8_t byteShift;           // Number of bits to be shifted out
volatile GPIO_TypeDef* transmitPort;  // GPIO port to be used for transmission
volatile uint8_t transmitPin;         // GPIO pin to be used for transmission

volatile uint8_t frameStarted, receivedByte, tLast;

static const struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} oswPins[4] = {
    {GPIOA, 1 << 4},
    {GPIOA, 1 << 6},
    {GPIOA, 1 << 7},
    {GPIOB, 1 << 1},
};

void oswSend(uint8_t* buf, uint8_t len, uint8_t pin) {
    while (sendCnt);
    transmitPort = oswPins[pin].port;  // Set GPIO port to be used for transmission
    transmitPin = oswPins[pin].pin;    // Set GPIO pin to be used for transmission
    sendPoint = buf;                   // Set pointer to the buffer to be sent
    sendCnt = len;                     // Set number of bytes to be sent
    byteShift = 0;
    TIM17->CR1 |= TIM_CR1_CEN;
}

void oswSendByte(uint8_t payload, uint8_t pin) {
    static uint8_t byteToSend;
    byteToSend = payload;
    oswSend(&byteToSend, 1, pin);
}

void TIM17_IRQHandler() {
    GPIOA->BSRR = GPIO_BSRR_BS_9;  // interrupt enter notification to LA
    static uint8_t parityBit;
    TIM17->SR = 0;                 // clear interrupt flag
    if (sendCnt) {                 // if there is something to send
        if (byteShift & 1) transmitPort->BSRR = transmitPin;
        else transmitPort->BRR = transmitPin;
        uint8_t bit = (byteShift<8)? ((*sendPoint) >> byteShift) & 1 : parityBit;
        TIM17->CNT =  bit? LOGIC_1_PERIOD : LOGIC_0_PERIOD;
        parityBit ^= bit;
        byteShift++;
        if (byteShift > 8) {
            parityBit=0;
            byteShift = 0xFF;
            sendPoint++;
            sendCnt--;
        }
    } else {
        transmitPort->BSRR = transmitPin;
        TIM17->CNT = 0xFFFF;
        TIM17->CR1 &= ~TIM_CR1_CEN;
    }
    frameStarted = 0;
    GPIOA->BSRR = GPIO_BSRR_BR_9;  // interrupt exit notification to LA
}

void EXTI0_1_IRQHandler() {
    // GPIOA->BSRR = GPIO_BSRR_BS_9;
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR |= EXTI_PR_PR1;
    }
    // GPIOA->BSRR = GPIO_BSRR_BR_9;
}

volatile uint8_t ReceivedByte,
                 ReceivedByteShift,
                 ReceivedBit;
volatile uint16_t timRX;

void EXTI4_15_IRQHandler() {
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR |= EXTI_PR_PR4;
    }
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR |= EXTI_PR_PR6;
    }
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR |= EXTI_PR_PR7;
        if(ReceivedByteShift>9){
            ReceivedByteShift=0;
            ReceivedByte=0;
        }
        if(ReceivedByteShift){
            if(ReceivedByteShift<9) {
                ReceivedBit = (TIM14->CNT < 0xFFFF-12);
                ReceivedByte |= (ReceivedBit << (ReceivedByteShift-1));
            }
            else{
                GPIOA->BSRR = GPIO_BSRR_BR_9;
                uartWrite(ReceivedByte);
            }
        }
        ReceivedByteShift++;
        TIM14->CNT = 0xFFFF - 20;
    }
}

void TIM14_IRQHandler() {
    TIM14->SR = 0;   
    ReceivedByteShift = 0;
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}