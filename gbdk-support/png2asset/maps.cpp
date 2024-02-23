#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "mttile.h"
#include "export.h"

#include "cmp_int_color.h"
#include "tiles.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "map_attributes.h"
#include "process_arguments.h"


void GetMap( PNG2AssetData* assetData)
{
    for(int y = 0; y < (int)assetData->image.h; y += assetData->image.tile_h)
    {
        for(int x = 0; x < (int)assetData->image.w; x += assetData->image.tile_w)
        {
            Tile tile(assetData->image.tile_w * assetData->image.tile_h);
            assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes);

            size_t idx;
            unsigned char props;

            // When both -keep_duplicate_tiles and source tilesets are used then
            // keep_duplicate_tiles should only apply to source tilesets, not the main image
            if ((assetData->args->keep_duplicate_tiles) &&
                ((assetData->args->has_source_tilesets == false) || (assetData->args->processing_mode == MODE_SOURCE_TILESET))) {
                assetData->tiles.push_back(tile);
                idx = assetData->tiles.size() - 1;
                props = assetData->args->props_default;
            }
            else
            {
                if(!FindTile(tile, idx, props,assetData))
                {
                    if ((assetData->args->processing_mode == MODE_MAIN_IMAGE) && (assetData->args->has_source_tilesets)) {
                        printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, (unsigned int)assetData->args->extra_tile_count + 1);
                        assetData->args->extra_tile_count++;
                        assetData->args->includeTileData = true;
                    }
                    assetData->tiles.push_back(tile);
                    idx = assetData->tiles.size() - 1;
                    props = assetData->args->props_default;

                    if(assetData->tiles.size() > 256 && assetData->args->pack_mode != Tile::SMS)
                        printf("Warning: found more than 256 tiles on x:%d,y:%d\n", x, y);

                    if(((assetData->tiles.size() + assetData->args->tile_origin) > 256) && (assetData->args->pack_mode != Tile::SMS))
                        printf("Warning: tile count (%d) + tile origin (%d) exceeds 256 at x:%d,y:%d\n", (unsigned int)assetData->tiles.size(), assetData->args->tile_origin, x, y);
                }
            }


            // Don't add map tiles and attributes for source tilesets
            if (assetData->args->processing_mode == MODE_MAIN_IMAGE) {
                assetData->map.push_back((unsigned char)idx + assetData->args->tile_origin);

                if(assetData->args->use_map_attributes)
                {
                    unsigned char pal_idx = assetData->image.data[y * assetData->image.w + x] >> assetData->args->bpp; //We can pick the palette from the first pixel of this tile
                    if(assetData->args->pack_mode == Tile::SGB)
                    {
                        props = props << 1; //Mirror flags in SGB are on bit 7
                        props |= (pal_idx + 4) << 2; //Pals are in bits 2,3,4 and need to go from 4 to 7
                        assetData->map.push_back(props); //Also they are stored within the map tiles
                    }
                    else if(assetData->args->pack_mode == Tile::SMS)
                    {
                        props = (props >> 4) | ((pal_idx & 1) << 3);
                        if(idx > 255)
                            props |= 1;
                        assetData->map.push_back(props);
                    }
                    else
                    {
                        props |= pal_idx;
                        assetData->map_attributes.push_back(props);
                    }
                }
            }
        }
    }


    // Don't add map tiles and attributes for source tilesets
    if (assetData->args->processing_mode == MODE_MAIN_IMAGE) {
        HandleMapAttributes( assetData);
    }
}

