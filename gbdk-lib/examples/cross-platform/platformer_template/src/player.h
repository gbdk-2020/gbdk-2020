#ifndef PLAYER_HEADER
#define PLAYER_HEADER
#include <gbdk/platform.h>

extern uint16_t playerX, playerY;
extern int16_t playerXVelocity, playerYVelocity;

void SetupPlayer() BANKED;
void UpdatePlayer() BANKED;

#endif