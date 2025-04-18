#ifndef _RTC_H_INCLUDE
#define _RTC_H_INCLUDE

#include <gbdk/platform.h>
#include <stdint.h>

volatile uint8_t AT(0x4000) RTC_SELECT_REG;
volatile uint8_t AT(0x6000) RTC_LATCH_REG;
volatile uint8_t AT(0xA000) RTC_VALUE_REG;

#define RTC_TIMER_STOP 0b01000000

typedef enum {
    RTC_VALUE_SEC = 0x08,
    RTC_VALUE_MIN,
    RTC_VALUE_HOUR,
    RTC_VALUE_DAY
} rtc_dateparts_e;

#define RTC_VALUE_FLAGS 0x0c

#define RAM_BANKS_ONLY 0x0fu
#define RAM_BANKS_AND_FLAGS 0xffu

extern volatile uint8_t _current_ram_bank;

inline void SWITCH_RAM_BANK(uint8_t bank, uint8_t mask) { SWITCH_RAM(_current_ram_bank = ((_current_ram_bank & ~mask) | (bank & mask))); }
inline void RTC_SELECT(uint8_t what) { SWITCH_RAM_BANK(what, RAM_BANKS_ONLY); }
inline void RTC_LATCH(void) { RTC_LATCH_REG = 0; RTC_LATCH_REG = 1; }

uint16_t RTC_GET(const rtc_dateparts_e part);
void RTC_SET(const rtc_dateparts_e part, const uint16_t v);
void RTC_START(const uint8_t start);

#endif