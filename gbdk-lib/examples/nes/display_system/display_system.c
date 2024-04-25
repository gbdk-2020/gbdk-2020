/*
    display_system.c

    Displays the graphics system gbdk-nes is running on. (NTSC/PAL/Dendy)
    
*/

#include <stdio.h>
#include <gbdk/platform.h>
#include <gbdk/font.h>
#include <gbdk/console.h>

const char* system_names[] = { "NTSC", "PAL", "Dendy", "Unknown" };

void main(void)
{
    font_t ibm_font;
    uint8_t system = get_display_system();
    // Init font system and load font
    font_init();
    ibm_font = font_load(font_ibm);
    DISPLAY_ON;
    gotoxy(4, 4);
    printf("System: %s", system_names[system]);
    vsync();
}
