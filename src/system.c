#include "system.h"

static inline void rccInit(void) {
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;                                      // Enable prefetch and 1 wait states

    RCC->CFGR = RCC_CFGR_PLLMUL12;                                                          // PLL x12

    RCC->CR |= RCC_CR_PLLON;                                                                // Enable PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));                                                     // Wait for PLL to be ready

    RCC->CFGR |= RCC_CFGR_SW_PLL;                                                           // Select PLL as system clock source
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));                                                // Wait for PLL to be selected

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN;            // Enable GPIOA, GPIOB, GPIOF
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_TIM17EN | RCC_APB2ENR_SYSCFGCOMPEN;  // Enable USART1, TIM17, SYSCFG
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;                                                    // Enable TIM14
}

static inline void gpioInit(void) {
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

    GPIOA->ODR |= GPIO_ODR_7 | GPIO_ODR_6 | GPIO_ODR_4;
    GPIOB->ODR |= GPIO_ODR_1;

    GPIOA->OTYPER |= /*GPIO_OTYPER_OT_9 |*/ GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_4;
    GPIOB->OTYPER |= GPIO_OTYPER_OT_1;

    GPIOA->MODER |= GPIO_MODER_MODER9_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1;
    GPIOB->MODER |= GPIO_MODER_MODER1_0;
    GPIOA->AFR[1] |= 0x00000020;
    GPIOA->AFR[0] |= 0x40001100;
}

void uartWrite(uint8_t d) {
    while (!(USART1->ISR & USART_ISR_TXE)) __NOP(); // Wait for TXE
    USART1->TDR = d;                                // Send data
}

static inline void uartInit(void) {
    #if (BAUD > 3000000)
        // high speed mode (oversampling by 8)
        USART1->BRR = ((2 * F_CPU / BAUD) & 0xFFF0) + (((2 * F_CPU / BAUD) & 0x000F) >> 1);
        USART1->CR1 = USART_CR1_OVER8;
    #else
        // normal mode (oversampling by 16)
        USART1->BRR = F_CPU / BAUD;
    #endif
    
    USART1->CR3 = USART_CR3_EIE; // error interrupt enable
    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    xdev_out(uartWrite);
}

static inline void extiInit(void) {
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB;
    // EXTICR2 is 0x0000 (PA4-PA7) by default
    RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGCOMPEN;  // turning off SYSCFG clock, need no more

    EXTI->RTSR = EXTI_RTSR_RT7 | EXTI_RTSR_RT6 | EXTI_RTSR_RT4 | EXTI_RTSR_RT1;
    EXTI->FTSR = EXTI_FTSR_FT7 | EXTI_FTSR_FT6 | EXTI_FTSR_FT4 | EXTI_FTSR_FT1;
    EXTI->IMR = EXTI_IMR_IM7 | EXTI_IMR_IM6 | EXTI_IMR_IM4 | EXTI_IMR_IM1;
}

static inline void tim14Init(void) {
    TIM14->PSC=50;
    TIM14->DIER |= TIM_DIER_UIE;    // enable update interrupt
    TIM14->CR1 |= TIM_CR1_CEN;
}

static inline void tim17Init(void) {
    TIM17->PSC=0;
    TIM17->DIER |= TIM_DIER_UIE;    // enable update interrupt
    // TIM17->CR1 |= TIM_CR1_CEN;      // enable timer
}

static inline void irqInit(void) {
    NVIC_SetPriority(SysTick_IRQn, 3);
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
    NVIC_SetPriority(EXTI0_1_IRQn, 2);

    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_EnableIRQ(TIM17_IRQn);
    NVIC_EnableIRQ(TIM14_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);
    SysTick_Config(F_CPU / 100);    // 100 Hz
}

void sysInit() {
    rccInit();
    gpioInit();
    uartInit();
    extiInit();
    tim14Init();
    tim17Init();
    irqInit();
}