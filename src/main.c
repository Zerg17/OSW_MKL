#include "osw.h"
#include "sysControl.h"
#include "system.h"
#include "ws2812.h"

uint8_t sendBuf[] = "zerg17 & yayayat\n";
// uint8_t sendBuf[] = {0b11110000, 0b11001100};

uint32_t tim1, tim2, tim3;
int main(void) {
    sysInit();
    while (1) {
        if (tim1 <= tick) {
            tim1 += 2;
                oswSend(sendBuf, sizeof(sendBuf), 2);
        }
        if (tim2 <= tick) {
            tim2 += 2000;
            // xprintf("test\n");
            oswSendByte(1,2);
        }
        __WFI();
        if(tim3<=tick) {
            tim3 += 2;
            for(uint8_t i=0; i<12; i++)
                led[i].r=tick/100+127;
            ws2812Write();
        }
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