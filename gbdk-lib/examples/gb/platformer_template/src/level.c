

#include <stdint.h>
#include <gbdk/platform.h>
#include "common.h"
#include "Map.h"
#include "Map2.h"
#include "Map3.h"
#include "MapTileset.h"
#include "MapTileset2.h"

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

uint8_t IsTileSolid(uint16_t worldX,uint16_t worldY){

    uint16_t column = worldX>>3;
    uint16_t row = worldY>>3;

    if(row>250)return FALSE;
    if(column>=currentLevelWidthInTiles)return TRUE;

    uint16_t index = column + row* currentLevelWidthInTiles;

    uint8_t tile = currentLevelMap[index];

    return tile>=currentLevelNonSolidTileCount;
}

void SetupCurrentLevel(){

    // We only have 3 levels
    // This will loop between them
    switch(currentLevel % 3){
        case 0:

            set_bkg_data(0,MapTileset_TILE_COUNT,MapTileset_tiles);

            currentLevelNonSolidTileCount=1;
            currentLevelWidth = Map_WIDTH;
            currentLevelHeight = Map_HEIGHT;
            currentLevelWidthInTiles = Map_WIDTH>>3;
            currentLevelHeightInTiles = Map_HEIGHT>>3;
            currentLevelMap= Map_map;

            break;
        case 1:

            set_bkg_data(0,MapTileset_TILE_COUNT,MapTileset_tiles);

            currentLevelNonSolidTileCount=1;
            currentLevelWidth = Map2_WIDTH;
            currentLevelHeight = Map2_HEIGHT;
            currentLevelWidthInTiles = Map2_WIDTH>>3;
            currentLevelHeightInTiles = Map2_HEIGHT>>3;
            currentLevelMap= Map2_map;

            break;
        case 2:

            set_bkg_data(0,MapTileset2_TILE_COUNT,MapTileset2_tiles);

            currentLevelNonSolidTileCount=1;
            currentLevelWidth = Map3_WIDTH;
            currentLevelHeight = Map3_HEIGHT;
            currentLevelWidthInTiles = Map3_WIDTH>>3;
            currentLevelHeightInTiles = Map3_HEIGHT>>3;
            currentLevelMap= Map3_map;

            break;
    }
}
