#include <gbdk/platform.h>
#include <gbdk/console.h>
#include <gb/isr.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <duck/laptop_io.h>

#include "megaduck_rtc.h"

static void use_rtc_data(duck_rtc_data_t * p_rtc);
static void send_some_rtc_data(duck_rtc_data_t * p_rtc);

duck_rtc_data_t rtc;


// Example of displaying RTC date info
static void use_rtc_data(duck_rtc_data_t * p_rtc) {

    const char * ampm_str[] = {"am", "pm"};
    const char * dow_str[]  = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    gotoxy(0,6);

    printf("Year:  %d  \n"
           "Month: %d  \n"
           "Day:   %d  \n"
           "DoW:   %s  \n"
           "Time:  %d:%d:%d %s   \n",
        (uint16_t)p_rtc->year,
        (uint16_t)p_rtc->mon,
        (uint16_t)p_rtc->day,
        (uint16_t)dow_str[p_rtc->weekday],

        (uint16_t)p_rtc->hour,
        (uint16_t)p_rtc->min,
        (uint16_t)p_rtc->sec,
        ampm_str[p_rtc->ampm] );
}


// This date and time used is just for example, they could be different values
static void send_some_rtc_data(duck_rtc_data_t * p_rtc) {

    // For this example set the first power-on defaults
    // to those used by the Spanish model laptop
    p_rtc->year    = 1993u;
    p_rtc->mon     = 6u; // June
    p_rtc->day     = 1u; // 1st
    p_rtc->weekday = 2u; // Tuesday

    p_rtc->ampm    = 0u; // AM
    p_rtc->hour    = 0u;
    p_rtc->min     = 0u;
    p_rtc->sec     = 0u;

    gotoxy(0,12);

    if (duck_io_set_rtc(p_rtc)) {
        printf("Send RTC: Success");
    }
    else {
        printf("Send RTC: Failed!");
    }
}


void main(void) {

    uint8_t gamepad;

    bool megaduck_laptop_init_ok = duck_io_laptop_init();

    SPRITES_8x8;
    SHOW_SPRITES;
    SHOW_BKG;
    printf("Initializing..\n");

	if (!megaduck_laptop_init_ok) {
        // If laptop hardware is not present then there isn't anything
        // for this program to do, so just idle in a loop
	    printf("Laptop not detected\n"
               "or Failed to Initialize");
        while(1) {
            vsync();
        }
	}

    // Otherwise laptop init succeeded
    printf("Laptop Detected!\n");

    printf("\n*SELECT to Send RTC\n Sys rom defaults");

	while(1) {
	    vsync();
        gamepad = joypad();

	    // Laptop serial command intervals below 20 msec may cause laptop hardware lockup
	    // Poll for RTC every other frame (~32 msec)
	    if (sys_time & 0x01u) {

            // Send RTC data to device if SELECT is pressed
            // otherwise Read RTC data
            if (gamepad & J_SELECT) {
                send_some_rtc_data(&rtc);

                // Wait until SELECT is released before continuing
                waitpadup();
            }
            else {
	            if (duck_io_get_rtc(&rtc)) {
	                use_rtc_data(&rtc);
                }
	        }
	    }
	}
}
