#include <gbdk/platform.h>
#include <stdio.h>
void main()
{
    // Wait 4 frames
    // For PAL SNES(SGB) this delay is required on startup
    for (uint8_t i = 4; i != 0; i--)
        vsync();
    uint8_t issgb = sgb_check();//sgb_check() is slow, so it's better to call it once and save the result to test with
    if ((_cpu == DMG_TYPE) && (!issgb))printf("This is a DMG! (OG Game Boy)");
    else if ((_cpu == MGB_TYPE) && (!issgb))printf("This is a MGB! (Game Boy Pocket / Light)");
    //Super Game Boys below
    else if ((_cpu == DMG_TYPE) && (issgb))printf("This is a SGB1!");
    else if ((_cpu == MGB_TYPE) && (issgb))printf("This is a SGB2!");
    //Game Boy Color
    else if ((_cpu == CGB_TYPE) && (!_is_GBA))printf("This is a CGB! (Game Boy Color)");
    //The GBA acts like a Game Boy Color. Adding !_is_GBA will make sure this only shows on a GBC
    else if(_is_GBA)printf("This is a GBA in CGB mode!");
    while(1)
    {
        vsync();
    }
}