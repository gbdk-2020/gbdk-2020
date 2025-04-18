#include <gbdk/platform.h>

#include <gbdk/console.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "mbc3_rtc.h"


// MBC3 RTC example
//
// Reads and displays the elapsed time since last
// reset from the Real Time Clock (RTC) on the MBC3 cart.
//
// It also allows resetting all the date/time components to zero.
//
// If your project does not use the included Makefile then
// make sure to set the MBC type in the cart header
// to 0x10 (RTC + SRAM) using this for lcc: -Wl-yt0x10
// Also set the number of cart SRAM banks: -Wm-ya4


// RTC data
typedef struct rtc_data_t {
    uint16_t  day;
    uint8_t   hour;
    uint8_t   min;
    uint8_t   sec;
} rtc_data_t;

rtc_data_t rtc;

static void read_and_show_rtc_data(rtc_data_t * p_rtc);
static void write_rtc_data(rtc_data_t * p_rtc);



// Example of reading and then displaying RTC date/time
static void read_and_show_rtc_data(rtc_data_t * p_rtc) {

    // Read RTC data
    RTC_LATCH();
    p_rtc->day  = RTC_GET(RTC_VALUE_DAY);
    p_rtc->hour = RTC_GET(RTC_VALUE_HOUR);
    p_rtc->min  = RTC_GET(RTC_VALUE_MIN);
    p_rtc->sec  = RTC_GET(RTC_VALUE_SEC);

    gotoxy(0,6);
    printf("Elapsed RTC Time:\n");
    printf("Days: %u  \n"
           "Time: %u:%u:%u   \n",
        (uint16_t)p_rtc->day,
        (uint16_t)p_rtc->hour,
        (uint16_t)p_rtc->min,
        (uint16_t)p_rtc->sec);
}


static void write_rtc_data(rtc_data_t * p_rtc) {

    // In this example just zero out the RTC data
    // so that when it's read back it will indicate
    // how much time has elapsed since setting it.
    p_rtc->day     = 0u; // Supported range: 0-511
    p_rtc->hour    = 0u; // Supported range: 0-23
    p_rtc->min     = 0u; // Supported range: 0-59
    p_rtc->sec     = 0u; // Supported range: 0-59

    RTC_SET(RTC_VALUE_DAY,  p_rtc->day);
    RTC_SET(RTC_VALUE_HOUR, p_rtc->hour);
    RTC_SET(RTC_VALUE_MIN,  p_rtc->min);
    RTC_SET(RTC_VALUE_SEC,  p_rtc->sec);

    gotoxy(0,11);
    printf("Set RTC to:\n");
    printf("Days: %u  \n"
           "Time: %u:%u:%u   \n",
        (uint16_t)p_rtc->day,
        (uint16_t)p_rtc->hour,
        (uint16_t)p_rtc->min,
        (uint16_t)p_rtc->sec);
}


static void clear_rtc_send_message(void) {

    gotoxy(0,11);
    printf("                  \n");
    printf("                  \n"
           "                  \n");
}


void main(void) {

    // Enable RAM for RTC register access and start the RTC ticking
    ENABLE_RAM;
    RTC_START(true);

    uint8_t gamepad;

    SPRITES_8x8;
    SHOW_SPRITES;
    SHOW_BKG;

    printf("RTC Date/Time Demo\n"
           "\n"
           "* Press SELECT to \n"
           "  set new RTC info.\n");

    uint8_t clear_notice_count = 0;
	while(1) {

        // Wait for the start of a new frame, then read the gamepad
	    vsync();
        gamepad = joypad();

        // Send RTC data to device if SELECT is pressed
        // otherwise check if it's time to read and show RTC data
        if (gamepad & J_SELECT) {
            write_rtc_data(&rtc);
            // Queue clearing the "sent rtc date/time" message after a few seconds
            clear_notice_count = 6;

            // Wait until SELECT is released before continuing
            waitpadup();
        }
        else {
            // Update Displayed RTC data every 32 frames (about twice per second)
            if ((sys_time & 0x1Fu) == 0u) {
                read_and_show_rtc_data(&rtc);

                if (clear_notice_count) {
                    clear_notice_count--;
                    if (clear_notice_count == 0) {
                        clear_rtc_send_message();
                    }
                }
            }
        }

    }

}
