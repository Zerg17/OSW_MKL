#include "system.h"

volatile uint8_t* sendPoint;
volatile uint8_t sendCnt;

volatile uint8_t bitShift;
volatile uint8_t byteShift;

void sendMsg(uint8_t* buf, uint8_t len){
    sendPoint=buf; 
    sendCnt=len;
}

void TIM17_IRQHandler(){
    GPIOA->BSRR = GPIO_BSRR_BS_9; // interrupt enter notification to LA
    TIM17->SR = 0;
    if(sendCnt){
        if(!bitShift){ 
            GPIOA->BSRR = GPIO_BSRR_BR_4;
            GPIOA->BSRR = GPIO_BSRR_BR_7;
            bitShift=1;
            TIM17->CNT=(((*sendPoint)>>byteShift)&1)?(65535-200):(65535-400);
        }else{
            GPIOA->BSRR = GPIO_BSRR_BS_4;
            GPIOA->BSRR = GPIO_BSRR_BS_7;
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
    GPIOA->BSRR = GPIO_BSRR_BR_9; //interrupt exit notification to LA

}