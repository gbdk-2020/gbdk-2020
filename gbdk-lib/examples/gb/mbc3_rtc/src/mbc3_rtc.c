#include <gbdk/platform.h>

#include "mbc3_rtc.h"

volatile uint8_t _current_ram_bank;

uint16_t RTC_GET(const rtc_dateparts_e part) {
    uint16_t v;
    RTC_SELECT(part);
    v = RTC_VALUE_REG;
    if (part == RTC_VALUE_DAY) {
        RTC_SELECT(RTC_VALUE_FLAGS);
        if (RTC_VALUE_REG & 0x01) v |= 0x0100u;
    }
    return v;
}

void RTC_SET(const rtc_dateparts_e part, const uint16_t v) {
    RTC_SELECT(part);
    RTC_VALUE_REG = v;
    if (part == RTC_VALUE_DAY) {
        RTC_SELECT(RTC_VALUE_FLAGS);
        RTC_VALUE_REG = (RTC_VALUE_REG & 0x0e) | (uint8_t)((v >> 8) & 0x01);
    }
}

void RTC_START(const uint8_t start) {
    RTC_SELECT(RTC_VALUE_FLAGS);
    if (start) RTC_VALUE_REG &= ~RTC_TIMER_STOP; else RTC_VALUE_REG |= RTC_TIMER_STOP;
}
