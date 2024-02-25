#include <gbdk/platform.h>
#include <stdint.h>
#include "common.h"

uint8_t joypadCurrent=0, joypadPrevious=0;

void WaitForStartOrA(){
    while(1){

        // Get the joypad input
        joypadPrevious = joypadCurrent;
        joypadCurrent = joypad();

        if((joypadCurrent & J_START) && !(joypadPrevious & J_START))break;
        if((joypadCurrent & J_A) && !(joypadPrevious & J_A))break;

        vsync();
        
    }
}