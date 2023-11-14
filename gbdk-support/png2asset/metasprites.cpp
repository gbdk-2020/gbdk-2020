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
            Tile tile(assetData->image.tile_h * assetData->image.tile_w);
            if(assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes))
            {
                size_t idx;
                unsigned char props;
                unsigned char pal_idx = assetData->image.data[y * assetData->image.w + x] >> 2; //We can pick the palette from the first pixel of this tile

                if(assetData->args->keep_duplicate_tiles)
                {
                    assetData->tiles.push_back(tile);
                    idx = assetData->tiles.size() - 1;
                    props = assetData->args->props_default;
                }
                else
                {
                    if(!FindTile(tile, idx, props, assetData))
                    {
                        if(assetData->args->source_tilesets.size() > 0) {
                            printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, (unsigned int)assetData->args->extra_tile_count + 1);
                            assetData->args->extra_tile_count++;
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

                mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
                last_x = x;
                last_y = y;
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
