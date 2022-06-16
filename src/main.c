#include "system.h"
#include "sysControl.h"
#include "osw.h"

uint8_t sendBuf[] = "zerg17 & yayayat\n";
// uint8_t sendBuf[] = {0b11110000, 0b11001100};

uint32_t tim1=0;
uint32_t tim2=0;
int main(void){
    sysInit();
    while(1){
        if(tim1<=tick){
            tim1+=2;
            // xprintf("%u", TIM17->CNT);
            sendMsg(sendBuf, sizeof(sendBuf));
        }
        // if(tim2<=tick){
        //     tim2+=200;
        //     xprintf("Uart test %u\n", tick);
        // }
        __WFI();
    }
}
