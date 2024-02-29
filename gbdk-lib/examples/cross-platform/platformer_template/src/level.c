#include <stdint.h>
#include <gbdk/platform.h>
#include "common.h"
#include "World1Area1.h"
#include "World1Area2.h"
#include "World2Area1.h"
#include "World1Waves.h"
#include "World1Tileset.h"
#include "World2Tileset.h"

#define WORLD1_SOLID_TILE_COUNT 17
#define WORLD2_SOLID_TILE_COUNT 68

BANKREF_EXTERN(World1Tileset)
BANKREF_EXTERN(World1Waves)
BANKREF_EXTERN(World2Tileset)
BANKREF_EXTERN(World1Area1)
BANKREF_EXTERN(World1Area2)
BANKREF_EXTERN(World1Area1)


// Instead of directly referencing our map constants, camera and physica code will reference these which can be changed between levels
uint16_t currentLevelWidth;
uint16_t currentLevelWidthInTiles;
uint16_t currentLevelHeight;
uint16_t currentLevelHeightInTiles;
uint8_t *currentLevelMap;
uint8_t currentLevelNonSolidTileCount;
uint8_t currentAreaBank;
void (*currentLevelVBL)() = 0;


// What is the current and next level we are on
// When they differ, we'll change levels
uint8_t currentLevel = 255;
uint8_t nextLevel = 0;

/**
 * @param worldX a (non-scaled) x position in the world
 * @param worldY a (non-scaled) y position in the world
 * @return uint8_t FALSE (0) if the tile is not solid, TRUE (1) if the tile is solid
 */
uint8_t IsTileSolid(uint16_t worldX,uint16_t worldY) NONBANKED{

    uint8_t _previous_bank = _current_bank;

    SWITCH_ROM(currentAreaBank);

    // Get convert the world position to a column and row by dividing by 8
    uint16_t column = worldX>>3;
    uint16_t row = worldY>>3;

    if(row>250||column>=currentLevelWidthInTiles){

        SWITCH_ROM(_previous_bank);
        return TRUE;
    }

    uint16_t index = column + row* currentLevelWidthInTiles;

    uint8_t tile = currentLevelMap[index];

    SWITCH_ROM(_previous_bank);

    return tile<currentLevelNonSolidTileCount;
}

void setBKGPalettes(uint8_t count, const palette_color_t *palettes) NONBANKED{
    // Set up color palettes
    #if defined(SEGA)
        __WRITE_VDP_REG(VDP_R2, R2_MAP_0x3800);
        __WRITE_VDP_REG(VDP_R5, R5_SAT_0x3F00);
        set_bkg_palette(0, count, palettes);
    #elif defined(GAMEBOY)
        if (_cpu == CGB_TYPE) {
            set_bkg_palette(OAMF_CGB_PAL0, count, palettes);
        }
    #elif defined(NINTENDO_NES)
        set_bkg_palette(0, count, palettes);
    #endif 
}

uint8_t waterFrame=0;

void World1VBL() NONBANKED{

    uint8_t _previous_bank = _current_bank;

    waterFrame++;

    uint8_t waterTrueFrame = (waterFrame>>3);

    if(waterTrueFrame>=8){
        waterTrueFrame=0;
        waterFrame=0;
    }


    // There are 4 water background tiles.
    const uint8_t waterBackgroundTileCount = 4;

    // They start at 22
    const uint8_t waterBackgroundTileStart = 22;

    SWITCH_ROM((BANK(World1Waves)));

    set_native_tile_data(waterBackgroundTileStart,waterBackgroundTileCount,World1Waves_tiles+waterBackgroundTileCount*16*waterTrueFrame);

    SWITCH_ROM(_previous_bank);
}


void SetupCurrentLevel() NONBANKED{

    // If we have a vertical blank handler set
    if(currentLevelVBL!=0){

        // disable interrupts and remove that handler
        CRITICAL{
            remove_VBL(currentLevelVBL);
        }

        // Reset our value
        currentLevelVBL=0;
    }
    

    uint8_t _previous_bank = _current_bank;


    // We only have 3 levels
    // This will loop between them
    switch(currentLevel % 3){
        case 0:



            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM(( BANK(World1Tileset)));

            set_native_tile_data(0,World1Tileset_TILE_COUNT,World1Tileset_tiles);
            setBKGPalettes(World1Tileset_PALETTE_COUNT,World1Tileset_palettes);
            
            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM((currentAreaBank = BANK(World1Area1)));

            currentLevelNonSolidTileCount=WORLD1_SOLID_TILE_COUNT;
            currentLevelWidth = World1Area1_WIDTH;
            currentLevelHeight = World1Area1_HEIGHT;
            currentLevelWidthInTiles = World1Area1_WIDTH>>3;
            currentLevelHeightInTiles = World1Area1_HEIGHT>>3;            
            currentLevelMap= World1Area1_map;

            // use a critical block to temporarily disable interrupts
            CRITICAL{

                // Save the pointer so we can remove it later
                add_VBL((currentLevelVBL=World1VBL));
            }


            break;
        case 1:


            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM((currentAreaBank = BANK(World1Tileset)));


            set_native_tile_data(0,World1Tileset_TILE_COUNT,World1Tileset_tiles);
            setBKGPalettes(World1Tileset_PALETTE_COUNT,World1Tileset_palettes);

            
            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM((currentAreaBank = BANK(World1Area2)));

            currentLevelNonSolidTileCount=WORLD1_SOLID_TILE_COUNT;
            currentLevelWidth = World1Area2_WIDTH;
            currentLevelHeight = World1Area2_HEIGHT;
            currentLevelWidthInTiles = World1Area2_WIDTH>>3;
            currentLevelHeightInTiles = World1Area2_HEIGHT>>3;
            currentLevelMap= World1Area2_map;

            // use a critical block to temporarily disable interrupts
            CRITICAL{

                // Save the pointer so we can remove it later
                add_VBL((currentLevelVBL=World1VBL));
            }

            break;
        case 2:


            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM((BANK(World2Tileset)));

            set_native_tile_data(0,World2Tileset_TILE_COUNT,World2Tileset_tiles);
            setBKGPalettes(World2Tileset_PALETTE_COUNT,World2Tileset_palettes);

            
            // Switch to whichever bank our song is in
            // Not neccessary if the song is in bank 0
            SWITCH_ROM((currentAreaBank = BANK(World2Area1)));

            currentLevelNonSolidTileCount=WORLD2_SOLID_TILE_COUNT;
            currentLevelWidth = World2Area1_WIDTH;
            currentLevelHeight = World2Area1_HEIGHT;
            currentLevelWidthInTiles = World2Area1_WIDTH>>3;
            currentLevelHeightInTiles = World2Area1_HEIGHT>>3;
            currentLevelMap= World2Area1_map;

            break;
    }
    SWITCH_ROM(_previous_bank);
}
