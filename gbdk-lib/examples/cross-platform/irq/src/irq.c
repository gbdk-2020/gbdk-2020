#include <gbdk/platform.h>
#include <gbdk/console.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// counters are 16-bit so we need a mutual exclusion access
unsigned int vbl_cnt, tim_cnt;

void vbl(void)
{
  // Upon IRQ, interrupts are automatically disabled 
  vbl_cnt++;
}

#if defined(NINTENDO_NES)
// For NES make sure to wrap TIM interrupt handlers with the nooverlay pragma.
// This avoids interrupts accidentally overwriting variables in the overlay segment that were being used when the NMI occurred.
#pragma save
#pragma nooverlay
#endif
void tim(void)
{
  // Upon IRQ, interrupts are automatically disabled
  tim_cnt++;
}
#if defined(NINTENDO_NES)
#pragma restore
#endif

void print_counter(void)
{
    unsigned int cnt;

    // Ensure mutual exclusion 
    CRITICAL { 
        cnt = tim_cnt; 
    }

    printf(" TIM %u", cnt);
    gotoxy(9, posy());

    // Ensure mutual exclusion
    CRITICAL { 
        cnt = vbl_cnt; 
    }

    printf("- VBL %u\n", cnt);
}

void main(void)
{
    // Ensure mutual exclusion
    CRITICAL {
        vbl_cnt = tim_cnt = 0;
        add_VBL(vbl);
        add_TIM(tim);
    }

#if defined(NINTENDO)
    // Set TMA to divide clock by 0x100
    TMA_REG = 0x00U;
    // Set clock to 4096 Hertz 
    TAC_REG = 0x04U;
#elif defined(SEGA) || defined(NINTENDO_NES)
    TMA_REG = 0xFCU;
#endif

    // Handle VBL and TIM interrupts
    set_interrupts(VBL_IFLAG | TIM_IFLAG);

    for(;;) {
        print_counter();
        // Loop for 60 frames (1 second)
        for(int i = 0; i < 60; i++) {
            vsync();
        }
    }
}
