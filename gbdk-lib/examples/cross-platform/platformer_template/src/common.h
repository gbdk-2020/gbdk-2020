#ifndef COMMON_HEADER
#define COMMON_HEADER


extern uint8_t joypadCurrent, joypadPrevious;

void WaitForStartOrA();
void ShowCentered(uint8_t widthInTiles,uint8_t heightInTiles,uint8_t bank, uint8_t* tileData, uint8_t tileCount, uint8_t* mapData) NONBANKED;
#endif