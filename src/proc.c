#include "proc.h"
#include "ws2812.h"
#include "sysControl.h"
#include "system.h"

uint8_t button;
uint8_t mode=15;

void proc(void){
    if((GPIOF->IDR&1)!=button){
        button^=1;
        if(button)mode=(mode+1)%8;
    }

    for(uint8_t i=0; i<12; i++) led[i].c=0;
    led[power_led].g=64;

    switch(mode){
        case 0: led[xor_led].c=0x100000; break;
        case 1: led[gt_led].c=0x001000; break;
        case 2: led[osc_led].c=0x101000; break;
        case 3: led[and_led].c=0x000010; break;
        case 4: led[not_led].c=0x100010; break;
        case 5: led[or_led].c=0x001010; break;
        case 6: led[dtr_led].c=0x101010; break;
    }
    if(mode==7){
        for(uint8_t i=0; i<12; i++) led[i].c=((tick%400)>200)?0x101010:0;
    }
    ws2812Write();
}