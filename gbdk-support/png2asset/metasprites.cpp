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
#include "tiles.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"

void GetAllMetasprites() {
	//Extract metasprites
	for(int y = 0; y < (int)image.h; y += sprite_h)
	{
		for(int x = 0; x < (int)image.w; x += sprite_w)
		{
			GetMetaSprite(x, y, sprite_w, sprite_h, pivot_x, pivot_y);
		}
	}
}


void GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y)
{
	int last_x = _x + pivot_x;
	int last_y = _y + pivot_y;

	sprites.push_back(MetaSprite());
	MetaSprite& mt_sprite = sprites.back();
	for(int y = _y; y < _y + _h && y < (int)image.h; y += image.tile_h)
	{
		for(int x = _x; x < _x + _w && x < (int)image.w; x += image.tile_w)
		{
			Tile tile(image.tile_h * image.tile_w);
			if(image.ExtractTile(x, y, tile, sprite_mode, export_as_map, use_map_attributes))
			{
				size_t idx;
				unsigned char props;
				unsigned char pal_idx = image.data[y * image.w + x] >> 2; //We can pick the palette from the first pixel of this tile

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
					}
				}

				props |= pal_idx;

				// Scale up index based on 8x8 tiles-per-hardware sprite
				if(sprite_mode == SPR_8x16)
					idx *= 2;
				else if(sprite_mode == SPR_16x16_MSX)
					idx *= 4;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
				last_x = x;
				last_y = y;
			}
		}
	}
}
