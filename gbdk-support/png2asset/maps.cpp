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


void GetMap(vector< SetPal > palettes)
{
	for(int y = 0; y < (int)image.h; y += image.tile_h)
	{
		for(int x = 0; x < (int)image.w; x += image.tile_w)
		{
			Tile tile(image.tile_w * image.tile_h);
			image.ExtractTile(x, y, tile, sprite_mode, export_as_map, use_map_attributes);

			size_t idx;
			unsigned char props;

			if(keep_duplicate_tiles)
			{
				tiles.push_back(tile);
				idx = tiles.size() - 1;
				props = props_default;
			}
			else
			{
				if(!FindTile(tile, idx, props))
				{
					if(use_source_tileset) {
						printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, extra_tile_count + 1);
						extra_tile_count++;
						includeTileData = true;
					}
					tiles.push_back(tile);
					idx = tiles.size() - 1;
					props = props_default;

					if(tiles.size() > 256 && pack_mode != Tile::SMS)
						printf("Warning: found more than 256 tiles on x:%d,y:%d\n", x, y);

					if(((tiles.size() + tile_origin) > 256) && (pack_mode != Tile::SMS))
						printf("Warning: tile count (%d) + tile origin (%d) exceeds 256 at x:%d,y:%d\n", (unsigned int)tiles.size(), tile_origin, x, y);
				}
			}

			map.push_back((unsigned char)idx + tile_origin);

			if(use_map_attributes)
			{
				unsigned char pal_idx = image.data[y * image.w + x] >> bpp; //We can pick the palette from the first pixel of this tile
				if(pack_mode == Tile::SGB)
				{
					props = props << 1; //Mirror flags in SGB are on bit 7
					props |= (pal_idx + 4) << 2; //Pals are in bits 2,3,4 and need to go from 4 to 7
					map.push_back(props); //Also they are stored within the map tiles
				}
				else if(pack_mode == Tile::SMS)
				{
					props = props >> 4;
					if(idx > 255)
						props |= 1;
					map.push_back(props);
				}
				else
				{
					props |= pal_idx;
					map_attributes.push_back(props);
				}
			}

		}
	}



	HandleMapAttributes(palettes);
}

