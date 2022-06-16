#include "sysControl.h"
#include "system.h"

uint32_t volatile tick=0;
uint32_t sec_d=0;
uint32_t volatile sec=0;

void SysTick_Handler(void) {
    tick++;
    if(++sec_d==100){
        sec_d=0;
        sec++;
    }

}

uint16_t t1;
void EXTI4_15_IRQHandler(){
    GPIOA->BSRR = GPIO_BSRR_BS_9;
    uint16_t t = TIM17->CNT;
    if(EXTI->PR & EXTI_PR_PR1){
        EXTI->PR |= EXTI_PR_PR1;
    }
    if(EXTI->PR & EXTI_PR_PR4){
        EXTI->PR |= EXTI_PR_PR4;
        if(GPIOA->IDR & GPIO_IDR_4){
            uint16_t tu=t-t1;
            USART1->TDR=tu<12;
        } else t1=t;
    }
    if(EXTI->PR & EXTI_PR_PR6){
        EXTI->PR |= EXTI_PR_PR6;
    }
    if(EXTI->PR & EXTI_PR_PR7){
        EXTI->PR |= EXTI_PR_PR7;
    }
    GPIOA->BSRR = GPIO_BSRR_BR_9;
}
