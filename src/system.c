#include "system.h"

static inline void rccInit(){
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN;
}

static inline void gpioInit(){
    // PA9  - WS2812        - TIM1_CH2
    // PA7  - IO3           - GPIO_OD(EXTI4_15)
    // PA6  - IO2           - GPIO_OD(EXTI4_15)
    // PA4  - IO1           - GPIO_OD(EXTI4_15)
    // PA3  - DEBUG_RX      - USART1_RX
    // PA2  - DEBUG_TX      - USART1_TX
    // PB1  - IO4           - GPIO_OD(EXTI4_15)
    // PF0  - BUTTON        - GPIO_IN

    // 0 - GPIO_OUT
    // 1 - Alternate function
    // Msk - Analog mode

    GPIOA->OTYPER |= GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_4;
    GPIOB->OTYPER |= GPIO_OTYPER_OT_1;

    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER7_0 |GPIO_MODER_MODER6_0 |GPIO_MODER_MODER4_0 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1;
    GPIOB->MODER |= GPIO_MODER_MODER1_0;
    GPIOA->AFR[1]|= 0x00000020;
    GPIOA->AFR[0]|= 0x00001100;
}

void uartWrite(uint8_t d){
    while(!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR=d;
}

static inline void uartInit(){
    USART1->BRR = (F_CPU+BAUD/2)/BAUD;
    USART1->CR1 = USART_CR1_TE;
    USART1->CR1 |= USART_CR1_UE;
    xdev_out(uartWrite);
}

static inline void extiInit(){
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB;
    EXTI->RTSR = EXTI_RTSR_RT7 | EXTI_RTSR_RT6 | EXTI_RTSR_RT4 | EXTI_RTSR_RT1;
    EXTI->FTSR = EXTI_FTSR_FT7 | EXTI_FTSR_FT6 | EXTI_FTSR_FT4 | EXTI_FTSR_FT1;
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

static inline void tim14Init(){
    TIM14->PSC = 7;
    TIM14->CR1 |= TIM_CR1_CEN;
}

void sysInit(){
    rccInit();
    gpioInit();
    uartInit();
    tim14Init();
    NVIC_SetPriority(SysTick_IRQn, 3);
    SysTick_Config(F_CPU/100);
}