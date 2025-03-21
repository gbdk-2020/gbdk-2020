#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <fstream>
#include <cstdint>

#include "mttile.h"

#include "cmp_int_color.h"
#include "metasprites.h"
#include "tiles.h"

#include "png2asset.h"
#include "image_utils.h"
#include "process_arguments.h"

using namespace std;

void ProcessMetaspriteTile(int _x, int _y, int x, int y, int* last_x, int* last_y, int _w, int _h, int pivot_x, int pivot_y, PNG2AssetData* assetData, int palleteIndex, MetaSprite& mt_sprite) {
    Tile tile(assetData->image.tile_h * assetData->image.tile_w);

    // For sprites, unlike maps, Tiles are only kept and processed if NOT Empty
    // This default behavior can be overridden with -spr_no_optimize (which sets keep_empty_sprite_tiles and keep_duplicate_tiles to TRUE)
    bool tile_not_empty = (assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes, assetData->args->bpp, palleteIndex));
    if(tile_not_empty || (assetData->args->keep_empty_sprite_tiles && palleteIndex == 0))
    {
        if(palleteIndex > 0) {

            printf("adding  layer (%d) for tile at %d,%d\n", palleteIndex + 1, x, y);
        }

        size_t idx;
        unsigned char props;

        // If a specific palette was specified, use it. 
        // Otherwise, We can pick the palette from the first pixel of this tile
        unsigned char pal_idx = palleteIndex!=-1 ?  palleteIndex  : assetData->image.data[y * assetData->image.w + x] >> assetData->args->bpp; 


        // When both -keep_duplicate_tiles and source tilesets are used then
        // keep_duplicate_tiles should only apply to source tilesets, not the main image
        if((assetData->args->keep_duplicate_tiles) && (assetData->args->has_source_tilesets == false)) {
            assetData->tiles.push_back(tile);
            idx = assetData->tiles.size() - 1;
            props = assetData->args->props_default;
        }
        else
        {
            if(!FindTile(tile, idx, props, assetData->tiles, assetData))
            {
                if(assetData->args->has_source_tilesets) {
                    printf("found a tile not in the source tileset at %d,%d\n", x, y);
                    assetData->args->includeTileData = true;
                }
                assetData->tiles.push_back(tile);
                idx = assetData->tiles.size() - 1;
                props = assetData->args->props_default;
            }
        }

        props |= pal_idx;

        // Scale up index based on 8x8 tiles-per-hardware sprite
        if(assetData->args->sprite_mode == SPR_8x16)
            idx *= 2;
        else if(assetData->args->sprite_mode == SPR_16x16_MSX)
            idx *= 4;

        mt_sprite.push_back(MTTile(x - *last_x, y - *last_y, (unsigned char)idx, props));
        *last_x = x;
        *last_y = y;
    }
}

void GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y, PNG2AssetData* assetData)
{
    int last_x = _x + pivot_x;
    int last_y = _y + pivot_y;

    assetData->sprites.push_back(MetaSprite());
    MetaSprite& mt_sprite = assetData->sprites.back();
    for(int y = _y; y < _y + _h && y < (int)assetData->image.h; y += assetData->image.tile_h)
    {
        for(int x = _x; x < _x + _w && x < (int)assetData->image.w; x += assetData->image.tile_w)
        {
            if(assetData->args->enable_layered_sprites) {


                int paletteCount = (int)assetData->args->max_palettes;
                for(int palleteIndex = 0; palleteIndex < paletteCount; palleteIndex++) {

                    ProcessMetaspriteTile(_x, _y,x,y, &last_x, &last_y, _w, _h, pivot_x, pivot_y, assetData, palleteIndex, mt_sprite);
                }
            }
            else {

                ProcessMetaspriteTile(_x,_y,x, y, &last_x, &last_y, _w, _h, pivot_x, pivot_y, assetData, -1, mt_sprite);
            }
        }
    }
}


void GetAllMetasprites(PNG2AssetData* assetData) {
    //Extract metasprites
    for(int y = 0; y < (int)assetData->image.h; y += (unsigned int)assetData->args->spriteSize.height)
    {
        for(int x = 0; x < (int)assetData->image.w; x += (unsigned int)assetData->args->spriteSize.width)
        {
            GetMetaSprite(x, y, (unsigned int)assetData->args->spriteSize.width, (unsigned int)assetData->args->spriteSize.height, assetData->args->pivot.x, assetData->args->pivot.y, assetData);
        }
    }
}
