#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/laptop_io.h>

#include "megaduck_rtc.h"

// These BCD conversions are for simplicity, in
// actual programs for performance it may be
// optimal to use the Game Boy / GBDK BCD features.
static uint8_t bcd_to_u8(uint8_t i)
{
    return (i & 0xFu) + ((i >> 4) * 10u);
}

static uint8_t u8_to_bcd(uint8_t i)
{
    return (i % 10u) + ((i / 10u) << 4);
}

// Get RTC command reply (Peripheral -> Duck)
//     All values are in BCD format
//         Ex: Month = December = 12th month = 0x12 (NOT 0x0C)
// [0]: Length of reply (always 4)
// [1..8] RTC Data
// [9]: checksum
//
// RTC Data:
//    if (tm.tm_year > (2000 - 1900))
//        [1] = int_to_bcd(tm.tm_year - (2000 - 1900));
//    else
//        [1] = int_to_bcd(tm.tm_year); // Years in BCD since 1900 (tm_year is already in since 1900 format)
//    [2] = int_to_bcd(tm.tm_mon + 1);
//    [3] = int_to_bcd(tm.tm_mday);
//    [4] = int_to_bcd(tm.tm_wday); // DOW
//
//    [5] = int_to_bcd( (tm.tm_hour < 12) ? 0 : 1 ); // AMPM
//    [6] = int_to_bcd(tm.tm_hour % 12);
//    [7] = int_to_bcd(tm.tm_min);
//    [8] = int_to_bcd(tm.tm_sec);


// Send RTC data to the Laptop Hardware and handle the response
//
// Returns success or failure
bool duck_io_set_rtc(duck_rtc_data_t * p_rtc) {

    uint8_t year_to_send;
    // Calculate as number of years since either 1900 or 2000
    // Strip year off, handle
    if (p_rtc->year < 2000u) year_to_send = p_rtc->year - 1900u;
    else                           year_to_send = p_rtc->year - 2000u;

    duck_io_tx_buf[0] = u8_to_bcd(year_to_send);
    duck_io_tx_buf[1] = u8_to_bcd(p_rtc->mon);
    duck_io_tx_buf[2] = u8_to_bcd(p_rtc->day);
    duck_io_tx_buf[3] = u8_to_bcd(p_rtc->weekday);

    duck_io_tx_buf[4] = p_rtc->ampm;
    duck_io_tx_buf[5] = p_rtc->hour;
    duck_io_tx_buf[6] = p_rtc->min;
    duck_io_tx_buf[7] = p_rtc->sec;

    duck_io_tx_buf_len = DUCK_IO_LEN_RTC_SET;

    if (duck_io_send_cmd_and_buffer(DUCK_IO_CMD_SET_RTC)) {
        return true;
    } else {
        return false;
    }
}


// Request RTC data and handle the response
//
// Returns success or failure (RTC struct data not updated if polling failed)
// Raw RTC data is converted into BCD format
bool duck_io_get_rtc(duck_rtc_data_t * p_rtc) {

    if (duck_io_send_cmd_and_receive_buffer(DUCK_IO_CMD_GET_RTC)) {
        if (duck_io_rx_buf_len == DUCK_IO_LEN_RTC_GET) {

            // Translate raw RTC data in BCD format to decimal

            // For Year the 1992 wraparound is optional, but it's how
            // the Super Junior SameDuck emulation does it in order
            // to remain compatible with the MegaDuck Laptop System ROM
            // which defaults to 1993 on startup (so BCD 93 for year)
            // and supports years as early as 1992 (BCD 92) within an
            // 8 bit bcd number (max being 99 years)
            p_rtc->year = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_YEAR]);
            if (p_rtc->year >= 92) p_rtc->year += 1900u;
            else                p_rtc->year += 2000u;

            p_rtc->mon     = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_MON]);
            p_rtc->day     = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_DAY]);
            p_rtc->mon     = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_MON]);
            p_rtc->weekday = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_WEEKDAY]);

            p_rtc->ampm    = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_AMPM]);
            p_rtc->hour    = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_HOUR]);
            p_rtc->min     = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_MIN]);
            p_rtc->sec     = bcd_to_u8(duck_io_rx_buf[DUCK_IO_RTC_SEC]);
            return true;
        }
    }
    return false;
}
