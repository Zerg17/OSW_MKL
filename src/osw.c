#include "system.h"

volatile uint8_t* sendPoint;          // Pointer to the buffer to be sent
volatile uint8_t sendCnt;             // Number of bytes to be sent
volatile uint8_t byteShift;           // Number of bits to be shifted out
volatile GPIO_TypeDef* transmitPort;  // GPIO port to be used for transmission
volatile uint32_t transmitPin;         // GPIO pin to be used for transmission

static const struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} oswPins[4] = {
    {GPIOA, 1 << 4},
    {GPIOA, 1 << 6},
    {GPIOA, 1 << 7},
    {GPIOB, 1 << 1}
};

void oswSend(uint8_t* buf, uint8_t len, uint8_t pin) {
    GPIOA->BSRR = GPIO_BSRR_BS_9;
    while (sendCnt);
    transmitPort = oswPins[pin].port;  // Set GPIO port to be used for transmission
    transmitPin = oswPins[pin].pin;    // Set GPIO pin to be used for transmission
    EXTI->IMR &= ~oswPins[pin].pin;
    sendPoint = buf;                   // Set pointer to the buffer to be sent
    sendCnt = len;                     // Set number of bytes to be sent
    byteShift = 0;
    TIM17->CR1 |= TIM_CR1_CEN;
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}

void oswSendByte(uint8_t payload, uint8_t pin) {
    static uint8_t byteToSend;
    byteToSend = payload;
    oswSend(&byteToSend, 1, pin);
}

void TIM17_IRQHandler() {
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
        EXTI->IMR |= transmitPin;
        TIM17->CNT = 0xFFFF;
        TIM17->CR1 &= ~TIM_CR1_CEN;
    }
}

static struct{
    uint8_t isActive,
            ReceivedByte,
            ReceivedByteShift,
            ParityBit;
    uint16_t timRX;
    uint8_t byteCounter;
} oswReceiver[4] = {
    {0, 0, 0xFF, 0, 0},
    {0, 0, 0xFF, 0, 0},
    {0, 0, 0xFF, 0, 0},
    {0, 0, 0xFF, 0, 0}
};

static inline void oswPinHandler(uint8_t pin){
    uint16_t t = TIM14->CNT;
    static uint8_t ReceivedBit;
    if (oswReceiver[pin].ReceivedByteShift>8){
        oswReceiver[pin].isActive = 2;
        oswReceiver[pin].ReceivedByte=0;
        oswReceiver[pin].ReceivedByteShift=0;
        oswReceiver[pin].ParityBit=0;
    }
    else{
        ReceivedBit = ((t - oswReceiver[pin].timRX) < LOGIC_THRESHOLD);
        oswReceiver[pin].ParityBit ^= ReceivedBit;
        if (oswReceiver[pin].ReceivedByteShift < 8) {
            oswReceiver[pin].ReceivedByte |= (ReceivedBit << (oswReceiver[pin].ReceivedByteShift));
        }
        else if(oswReceiver[pin].ReceivedByteShift == 8){
            // if(oswReceiver[pin].ParityBit)
            oswSendByte(oswReceiver[pin].ReceivedByte + 1, pin);
            uartWrite(oswReceiver[pin].ReceivedByte);
            //     uartWrite('>');
            // uartWrite(oswReceiver[pin].ReceivedByte);
            // uartWrite((uint8_t)(t - oswReceiver[pin].timRX));
        }
        oswReceiver[pin].ReceivedByteShift++;
    }
    oswReceiver[pin].timRX = t;
}

void EXTI0_1_IRQHandler() {
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR = EXTI_PR_PR1;
        oswPinHandler(3);
    }
}

void EXTI4_15_IRQHandler() {
    // GPIOA->BSRR = GPIO_BSRR_BS_9;
    if (EXTI->PR & EXTI_PR_PR4) {
        EXTI->PR = EXTI_PR_PR4;
        oswPinHandler(0);
    }
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR = EXTI_PR_PR6;
        oswPinHandler(1);
    }
    if (EXTI->PR & EXTI_PR_PR7) {
        EXTI->PR = EXTI_PR_PR7;
        oswPinHandler(2);
    }
    // GPIOA->BSRR = GPIO_BSRR_BR_9;
}

void oswReceiverReset(){
    TIM14->SR = 0; 
    for(uint8_t i=0; i<4; i++){
        if (oswReceiver[i].isActive) oswReceiver[i].isActive--;
        else oswReceiver[i].ReceivedByteShift = 0xFF;
    }
}
