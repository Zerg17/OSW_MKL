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

void EXTI4_15_IRQHandler(){
    uint16_t t = TIM14->CNT;
    (void)t;
    if(EXTI->PR & EXTI_PR_PR1){
        EXTI->PR |= EXTI_PR_PR1;
    }
    if(EXTI->PR & EXTI_PR_PR4){
        EXTI->PR |= EXTI_PR_PR4;
    }
    if(EXTI->PR & EXTI_PR_PR6){
        EXTI->PR |= EXTI_PR_PR6;
    }
    if(EXTI->PR & EXTI_PR_PR7){
        EXTI->PR |= EXTI_PR_PR7;
    }
}