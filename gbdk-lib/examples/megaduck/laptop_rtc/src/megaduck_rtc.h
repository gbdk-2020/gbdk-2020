#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEGADUCK_RTC_H
#define _MEGADUCK_RTC_H


// RTC data
typedef struct duck_rtc_data_t {
    uint16_t year;
    uint8_t  mon;
    uint8_t  day;
    uint8_t  weekday;

    uint8_t  ampm;
    uint8_t  hour;
    uint8_t  min;
    uint8_t  sec;
} duck_rtc_data_t;


bool duck_io_set_rtc(duck_rtc_data_t * p_rtc);
bool duck_io_get_rtc(duck_rtc_data_t * p_rtc);

#endif // _MEGADUCK_RTC_H
