#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "global.h"
#include "mttile.h"
#include "export.h"

#include "cmp_int_color.h"
#include "tiles.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "map_attributes.h"
#include "process_arguments.h"


void GetMap(vector< SetPal > palettes, PNG2AssetData* png2AssetData)
{
	for(int y = 0; y < (int)png2AssetData->image.h; y += png2AssetData->image.tile_h)
	{
		for(int x = 0; x < (int)png2AssetData->image.w; x += png2AssetData->image.tile_w)
		{
			Tile tile(png2AssetData->image.tile_w * png2AssetData->image.tile_h);
			png2AssetData->image.ExtractTile(x, y, tile, png2AssetData->sprite_mode, png2AssetData->export_as_map, png2AssetData->use_map_attributes);

			size_t idx;
			unsigned char props;

			if(png2AssetData->keep_duplicate_tiles)
			{
				png2AssetData->tiles.push_back(tile);
				idx = png2AssetData->tiles.size() - 1;
				props = png2AssetData->props_default;
			}
			else
			{
				if(!FindTile(tile, idx, props,png2AssetData))
				{
					if(png2AssetData->use_source_tileset) {
						printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, png2AssetData->extra_tile_count + 1);
						png2AssetData->extra_tile_count++;
						png2AssetData->includeTileData = true;
					}
					png2AssetData->tiles.push_back(tile);
					idx = png2AssetData->tiles.size() - 1;
					props = png2AssetData->props_default;

					if(png2AssetData->tiles.size() > 256 && png2AssetData->pack_mode != Tile::SMS)
						printf("Warning: found more than 256 tiles on x:%d,y:%d\n", x, y);

					if(((png2AssetData->tiles.size() + png2AssetData->tile_origin) > 256) && (png2AssetData->pack_mode != Tile::SMS))
						printf("Warning: tile count (%d) + tile origin (%d) exceeds 256 at x:%d,y:%d\n", (unsigned int)png2AssetData->tiles.size(), png2AssetData->tile_origin, x, y);
				}
			}

			png2AssetData->map.push_back((unsigned char)idx + png2AssetData->tile_origin);

			if(png2AssetData->use_map_attributes)
			{
				unsigned char pal_idx = png2AssetData->image.data[y * png2AssetData->image.w + x] >> png2AssetData->bpp; //We can pick the palette from the first pixel of this tile
				if(png2AssetData->pack_mode == Tile::SGB)
				{
					props = props << 1; //Mirror flags in SGB are on bit 7
					props |= (pal_idx + 4) << 2; //Pals are in bits 2,3,4 and need to go from 4 to 7
					png2AssetData->map.push_back(props); //Also they are stored within the map tiles
				}
				else if(png2AssetData->pack_mode == Tile::SMS)
				{
					props = props >> 4;
					if(idx > 255)
						props |= 1;
					png2AssetData->map.push_back(props);
				}
				else
				{
					props |= pal_idx;
					png2AssetData->map_attributes.push_back(props);
				}
			}

		}
	}



	HandleMapAttributes(palettes, png2AssetData);
}

