#include "osw.h"
#include "sysControl.h"
#include "system.h"
#include "ws2812.h"
#include "proc.h"

uint8_t sendBuf[] = "zerg17 & yayayat\n";
// uint8_t sendBuf[] = {0b11110000, 0b11001100};

uint32_t tim1, tim2, tim3, procTim;

static const struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} oswPins[4] = {
    {GPIOA, 1 << 4},
    {GPIOA, 1 << 6},
    {GPIOA, 1 << 7},
    {GPIOB, 1 << 1}
};

int main(void) {
    sysInit();
    while (1) {
        if (tim1 <= tick) {
            tim1 += 2;
                // oswSend(sendBuf, sizeof(sendBuf), 2);
        }
        if (tim2 <= tick) {
            tim2 += 2000;
            // xprintf("test\n");
            // oswSendByte(1,2);
            if(tim2/2000%2){
                for(uint8_t i=0; i<4; i++) oswPins[i].port->BSRR = oswPins[i].pin;
            }else{
                for(uint8_t i=0; i<4; i++) oswPins[i].port->BSRR = oswPins[i].pin<<16;
            }
        }
        
        // if(tim3<=tick) {
        //     tim3++;
        //     for(uint8_t i=0; i<12; i++){
        //         // led[i].r=tick/100+i;
        //         // led[i].g=tick/100+i;
        //         // led[i].b=tick/100+i;
        //         // led[i] = setHSVfast(tick+i*20, 255, GPIOF->IDR&1?64:0);
        //         led[i].r = 255;
        //         led[i].g = 255;
        //         led[i].b = 255;
        //     }
        //     // led[mode].b=255;
        //     ws2812Write();
        // }

        if(procTim<=tick){
            procTim+=2;
            proc();
        }
        __WFI();
    }
}

void USART1_IRQHandler() {
    if (USART1->ISR & USART_ISR_RXNE) {  //receive buffer is not empty
        uint8_t receivedChar = (uint8_t)(USART1->RDR);
        // oswSendByte(receivedChar, 2);
        uartWrite(receivedChar);
    }

    if (USART1->ISR & USART_ISR_FE) {  // Framing error
        USART1->ICR = USART_ICR_FECF;
    }
    if (USART1->ISR & USART_ISR_ORE) {  // Overrun error
        USART1->ICR = USART_ICR_ORECF;
    }
    if (USART1->ISR & USART_ISR_NE) {  // Noise error
        USART1->ICR = USART_ICR_NCF;
    }
}