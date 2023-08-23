#ifndef SAMPLE_PLAYER_H_INCLUDE
#define SAMPLE_PLAYER_H_INCLUDE

#include <gbdk/platform.h>

#define PSG_LATCH       0x80
#define PSG_DATA        0x40

#define PSG_CH0         0b00000000
#define PSG_CH1         0b00100000
#define PSG_CH2         0b01000000
#define PSG_CH3         0b01100000
#define PSG_VOLDDATA    0b00010000

void play_sample(uint8_t * sample, uint16_t size);

#endif