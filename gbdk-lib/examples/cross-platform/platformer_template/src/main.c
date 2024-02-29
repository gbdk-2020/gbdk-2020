#include <gbdk/platform.h>
#include <stdint.h>
#include "player.h"
#include "common.h"
#include "level.h"
#include "Camera.h"
#include "NextLevel.h"
#include "TitleScreen.h"

BANKREF_EXTERN(NextLevel)
BANKREF_EXTERN(TitleScreen)


void main(void)
{

    
    #ifdef NINTENDO
        // init palettes
        BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
        OBP0_REG =OBP1_REG =DMG_PALETTE(DMG_DARK_GRAY, DMG_BLACK, DMG_WHITE,  DMG_LITE_GRAY );
    #endif
    
	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;

    ShowCentered(TitleScreen_WIDTH,TitleScreen_HEIGHT,BANK(TitleScreen),TitleScreen_tiles,TitleScreen_TILE_COUNT,TitleScreen_map);

    WaitForStartOrA();

    // Make sure these are initially different so the "setupcurrentLevel" logic is triggered
    currentLevel=255;
    nextLevel=0;

    // Loop forever
    while(1) {

        // if we want to change levels
        if(nextLevel!=currentLevel){

            // if we're not starting the game (where currentLevel = 255)
            if(currentLevel!=255){

                ShowCentered(NextLevel_WIDTH,NextLevel_HEIGHT,BANK(NextLevel),NextLevel_tiles,NextLevel_TILE_COUNT,NextLevel_map);

                WaitForStartOrA();
            }

            // Update what our current level is
            currentLevel=nextLevel;

            // Setup the new level
            SetupCurrentLevel();

            // Draw the initial area
            // Draw one extra column to avoid a blank row when first scrolling.
            // If scrolling vertically also, you should draw one extra row as well.
            // The platformer template will only scroll horizontally
            SetCurrentLevelSubmap(0,0,DEVICE_SCREEN_WIDTH+1,DEVICE_SCREEN_HEIGHT);

            // Setup the player
            SetupPlayer();
        }

        // Get the joypad input
        joypadPrevious = joypadCurrent;
        joypadCurrent = joypad();

        UpdatePlayer();
        UpdateCamera();

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
