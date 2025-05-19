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


void ExtractTileset(PNG2AssetData* assetData, vector< Tile > & tileset, bool keep_duplicate_tiles) {

    // Placeholder vars needed for function call, they get discarded
    size_t idx;
    unsigned char props;

    for(int y = 0; y < (int)assetData->image.h; y += assetData->image.tile_h)
    {
        for(int x = 0; x < (int)assetData->image.w; x += assetData->image.tile_w)
        {
            // Get a tile from the image
            Tile tile(assetData->image.tile_w * assetData->image.tile_h);
            assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes, assetData->args->bpp);

            if (keep_duplicate_tiles)
                tileset.push_back(tile);
            else {
                if (!FindTile(tile, idx, props, tileset, assetData))
                    tileset.push_back(tile);
            }
        }
    }
}


static void checkWarnMapTileLimits(PNG2AssetData* assetData) {

    unsigned int maxUsedTileCount = (assetData->tiles.size() + assetData->args->tile_origin);
    unsigned int maxTilesWarnLimit = 256; // 256 is default for GB, NES

    // GB + Map Attributes implies GBC which has a secondary tile bank
    if (((assetData->args->pack_mode == Tile::GB) && assetData->args->use_map_attributes) ||
         (assetData->args->pack_mode == Tile::SMS)) {
        maxTilesWarnLimit = 512;
    }

    if (maxUsedTileCount > maxTilesWarnLimit) {
        printf("Warning: Tile count (%d) + tile origin (%d) = %d exceeds limit %d\n",
                (unsigned int)assetData->tiles.size(), (unsigned int)assetData->args->tile_origin,
                maxUsedTileCount, maxTilesWarnLimit);
    }

    // If using GBC, notify user about extra requirements to use > 256 tiles
    if ((maxUsedTileCount > 256) && ((assetData->args->pack_mode == Tile::GB) && assetData->args->use_map_attributes)) {
        printf("Warning: On GBC more then 256 tiles may require use of alternate tile bank. Tile count (%d) + tile origin (%d) = %d\n",
                (unsigned int)assetData->tiles.size(), (unsigned int)assetData->args->tile_origin, maxUsedTileCount);
    }
}


void GetMap(PNG2AssetData* assetData)
{
    for(int y = 0; y < (int)assetData->image.h; y += assetData->image.tile_h)
    {
        for(int x = 0; x < (int)assetData->image.w; x += assetData->image.tile_w)
        {
            // Get a tile from the image
            Tile tile(assetData->image.tile_w * assetData->image.tile_h);
            assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes, assetData->args->bpp);

            size_t idx;
            unsigned char props;

            // If there is an entity tileset then always check that for a matching tile first
            if (assetData->args->has_entity_tileset && FindTile(tile, idx, props, assetData->entity_tiles, assetData)) {
                // Entity tilesets index counts from 255 down instead of 0 up,
                // so invert the tile ID on the 8 bit boundary
                idx = (256 - assetData->entity_tiles.size()) + idx;
                props = assetData->args->props_default;
            }
            // When both -keep_duplicate_tiles and source tilesets are used then
            // keep_duplicate_tiles should only apply to source tilesets, not the main image.
            // (Since keeping duplicate tiles from the main image would
            //  cause the identical tiles in a source tileset to be ignored)
            else if ((assetData->args->keep_duplicate_tiles) && (assetData->args->has_source_tilesets == false)) {
                // Save tile pattern data, don't try to deduplicate it
                assetData->tiles.push_back(tile);
                idx = assetData->tiles.size() - 1;
                props = assetData->args->props_default;
            }
            else {
                // Otherwise if the tile pattern has not been encountered before then save it
                if (!FindTile(tile, idx, props, assetData->tiles, assetData))
                {
                    if (assetData->args->has_source_tilesets) {
                        printf("found a tile not in the source tileset at %d,%d\n", x, y);
                        assetData->args->includeTileData = true;
                    }
                    assetData->tiles.push_back(tile);
                    idx = assetData->tiles.size() - 1;
                    props = assetData->args->props_default;
                }
            }


            // Creating map tile id and attributes entries is only when processing the the main image.
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
                     // If GBC and more than 256 tiles then add in tile bank num
                    if ((idx > 255) && (assetData->args->pack_mode == Tile::GB))
                        props |= CGB_BKGF_BANK1;
                    assetData->map_attributes.push_back(props);
                }
            }
        }
    }

    checkWarnMapTileLimits(assetData);
    HandleMapAttributes(assetData);
}

