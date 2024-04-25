/*
    display_system.c

    Displays the system gbdk is running on.
    
    This can be one of these three:
    * NTSC  (60Hz)
    * PAL   (50Hz)
    * Dendy (50Hz but with NTSC scanline cycle timing - only applicable to gbdk-nes port)
    
*/

#include <stdio.h>
#include <gbdk/platform.h>
#include <gbdk/font.h>
#include <gbdk/console.h>

//const char* system_names[] = { "NTSC", "PAL", "Dendy", "Unknown" };

const char* get_system_name(uint8_t system)
{
    switch(system)
    {
        case SYSTEM_NTSC:
            return "NTSC";
        case SYSTEM_PAL:
            return "PAL";
        case SYSTEM_DENDY:
            return "Dendy";
        default:
            return "Unknown";
    }
}

void main(void)
{
    font_t ibm_font;
    uint8_t system = get_system();
    // Init font system and load font
    font_init();
    ibm_font = font_load(font_ibm);
    DISPLAY_ON;
    gotoxy(4, 4);
    printf("System: %s", get_system_name(system)); //system_names[system]);
    vsync();
}
