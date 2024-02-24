#include <stdint.h>
#include <gbdk/platform.h>
#include "common.h"
#include "World1Area1.h"
#include "World1Area2.h"
#include "World2Area1.h"
#include "World1Tileset.h"
#include "World2Tileset.h"

#define WORLD1_SOLID_TILE_COUNT 17
#define WORLD2_SOLID_TILE_COUNT 68

// Instead of directly referencing our map constants, camera and physica code will reference these which can be changed between levels
uint16_t currentLevelWidth;
uint16_t currentLevelWidthInTiles;
uint16_t currentLevelHeight;
uint16_t currentLevelHeightInTiles;
uint8_t *currentLevelMap;
uint8_t currentLevelNonSolidTileCount;

// What is the current and next level we are on
// When they differ, we'll change levels
uint8_t currentLevel = 255;
uint8_t nextLevel = 0;

/**
 * @param worldX a (non-scaled) x position in the world
 * @param worldY a (non-scaled) y position in the world
 * @return uint8_t FALSE (0) if the tile is not solid, TRUE (1) if the tile is solid
 */
uint8_t IsTileSolid(uint16_t worldX,uint16_t worldY){

    // Get convert the world position to a column and row by dividing by 8
    uint16_t column = worldX>>3;
    uint16_t row = worldY>>3;

    if(row>250)return FALSE;
    if(column>=currentLevelWidthInTiles)return TRUE;

    uint16_t index = column + row* currentLevelWidthInTiles;

    uint8_t tile = currentLevelMap[index];

    return tile<currentLevelNonSolidTileCount;
}

void SetupCurrentLevel(){

    // We only have 3 levels
    // This will loop between them
    switch(currentLevel % 3){
        case 0:

            set_bkg_data(0,World1Tileset_TILE_COUNT,World1Tileset_tiles);

            currentLevelNonSolidTileCount=WORLD1_SOLID_TILE_COUNT;
            currentLevelWidth = World1Area1_WIDTH;
            currentLevelHeight = World1Area1_HEIGHT;
            currentLevelWidthInTiles = World1Area1_WIDTH>>3;
            currentLevelHeightInTiles = World1Area1_HEIGHT>>3;
            currentLevelMap= World1Area1_map;

            break;
        case 1:

            set_bkg_data(0,World1Tileset_TILE_COUNT,World1Tileset_tiles);

            currentLevelNonSolidTileCount=WORLD1_SOLID_TILE_COUNT;
            currentLevelWidth = World1Area2_WIDTH;
            currentLevelHeight = World1Area2_HEIGHT;
            currentLevelWidthInTiles = World1Area2_WIDTH>>3;
            currentLevelHeightInTiles = World1Area2_HEIGHT>>3;
            currentLevelMap= World1Area2_map;

            break;
        case 2:

            set_bkg_data(0,World2Tileset_TILE_COUNT,World2Tileset_tiles);

            currentLevelNonSolidTileCount=WORLD2_SOLID_TILE_COUNT;
            currentLevelWidth = World2Area1_WIDTH;
            currentLevelHeight = World2Area1_HEIGHT;
            currentLevelWidthInTiles = World2Area1_WIDTH>>3;
            currentLevelHeightInTiles = World2Area1_HEIGHT>>3;
            currentLevelMap= World2Area1_map;

            break;
    }
}
