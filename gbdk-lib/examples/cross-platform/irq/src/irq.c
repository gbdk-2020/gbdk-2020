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
    uint8_t double_speed_mode = 0;
#if defined(NINTENDO)
    set_default_palette();
    if (_cpu == CGB_TYPE) {
        // Use double-speed mode for GBC
        cpu_fast();
        double_speed_mode = 1;
    }
#endif

#if defined(NINTENDO_NES)
    // NES TIM emulation is based on GBC's double-speed mode
    double_speed_mode = 1;
#endif

    // Ensure mutual exclusion
    CRITICAL {
        vbl_cnt = tim_cnt = 0;
        add_VBL(vbl);
        add_TIM(tim);
    }

#if defined(NINTENDO) || defined(NINTENDO_NES)
    // Set TMA to divide clock by 0x80 (0x100 for GBC)
    TMA_REG = double_speed_mode ? 0x00U : 0x80;
    // Set clock to 4096 Hertz (8192 Hertz for GBC)
    TAC_REG = 0x04U;
#elif defined(SEGA)
    TMA_REG = 0xFCU;
#endif

    // Handle VBL and TIM interrupts
    set_interrupts(VBL_IFLAG | TIM_IFLAG);

    for(;;) {
        uint8_t num_frames = (get_system() == SYSTEM_50HZ) ? 50 : 60;
        print_counter();
        // Loop for 1 second
        for(int i = 0; i < num_frames; i++) {
            vsync();
        }
    }
}
