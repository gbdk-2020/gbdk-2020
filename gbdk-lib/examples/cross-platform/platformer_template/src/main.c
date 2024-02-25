#include <gbdk/platform.h>
#include <stdint.h>
#include "player.h"
#include "common.h"
#include "title_screen.h"
#include "next_level.h"
#include "level.h"
#include "Camera.h"

void main(void)
{
    
    #ifdef NINTENDO
        // init palettes
        BGP_REG = OBP0_REG = OBP1_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
    #endif
    
	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;

    ShowTitleScreen();

    // Make sure these are initially different so the "setupcurrentLevel" logic is triggered
    currentLevel=255;
    nextLevel=0;

    // Loop forever
    while(1) {

        // if we want to change levels
        if(nextLevel!=currentLevel){

            // if we're not starting the game (where currentLevel = 255)
            if(currentLevel!=255){

                ShowNextLevel();
            }

            // Update what our current level is
            currentLevel=nextLevel;

            // Setup the new level
            SetupCurrentLevel();

            // Make the initial draw for the camera
            InitialCameraDraw();

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
