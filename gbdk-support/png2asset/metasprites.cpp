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

void GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y, PNG2AssetData* png2AssetData)
{
	int last_x = _x + pivot_x;
	int last_y = _y + pivot_y;

	png2AssetData->sprites.push_back(MetaSprite());
	MetaSprite& mt_sprite = png2AssetData->sprites.back();
	for(int y = _y; y < _y + _h && y < (int)png2AssetData->image.h; y += png2AssetData->image.tile_h)
	{
		for(int x = _x; x < _x + _w && x < (int)png2AssetData->image.w; x += png2AssetData->image.tile_w)
		{
			Tile tile(png2AssetData->image.tile_h * png2AssetData->image.tile_w);
			if(png2AssetData->image.ExtractTile(x, y, tile, png2AssetData->sprite_mode, png2AssetData->export_as_map, png2AssetData->use_map_attributes))
			{
				size_t idx;
				unsigned char props;
				unsigned char pal_idx = png2AssetData->image.data[y * png2AssetData->image.w + x] >> 2; //We can pick the palette from the first pixel of this tile

				if(png2AssetData->keep_duplicate_tiles)
				{
					png2AssetData->tiles.push_back(tile);
					idx = png2AssetData->tiles.size() - 1;
					props = png2AssetData->props_default;
				}
				else
				{
					if(!FindTile(tile, idx, props, png2AssetData))
					{
						if(png2AssetData->use_source_tileset) {
							printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, png2AssetData->extra_tile_count + 1);
							png2AssetData->extra_tile_count++;
							png2AssetData->includeTileData = true;
						}
						png2AssetData->tiles.push_back(tile);
						idx = png2AssetData->tiles.size() - 1;
						props = png2AssetData->props_default;
					}
				}

				props |= pal_idx;

				// Scale up index based on 8x8 tiles-per-hardware sprite
				if(png2AssetData->sprite_mode == SPR_8x16)
					idx *= 2;
				else if(png2AssetData->sprite_mode == SPR_16x16_MSX)
					idx *= 4;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
				last_x = x;
				last_y = y;
			}
		}
	}
}


void GetAllMetasprites(PNG2AssetData* png2AssetData) {
	//Extract metasprites
	for(int y = 0; y < (int)png2AssetData->image.h; y += png2AssetData->sprite_h)
	{
		for(int x = 0; x < (int)png2AssetData->image.w; x += png2AssetData->sprite_w)
		{
			GetMetaSprite(x, y, png2AssetData->sprite_w, png2AssetData->sprite_h, png2AssetData->pivot_x, png2AssetData->pivot_y, png2AssetData);
		}
	}
}
