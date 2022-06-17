#include "system.h"
#include "sysControl.h"
#include "osw.h"

uint8_t sendBuf[] = "zerg17 & yayayat\n";
// uint8_t sendBuf[] = {0b11110000, 0b11001100};

uint32_t tim1=0;
int main(void){
    sysInit();
    while(1){
        if(tim1<=tick){
            tim1+=2;
            if(tim1%4>2)oswSend(sendBuf, sizeof(sendBuf), 0);
            else oswSend(sendBuf, sizeof(sendBuf), 1);
        }
        __WFI();
    }
}
