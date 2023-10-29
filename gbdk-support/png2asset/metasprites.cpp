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

void GetMetaspriteLayeredTiles(Tile& tile, MetaSprite& mt_sprite, int* last_x, int* last_y, int x, int y, PNG2AssetData* assetData, int *layeredSpritesCount) {

	size_t idx;
	unsigned char props;

	if(tile.extraPaletteInfo.size() > 0) {

		for(int z = 0; z < tile.extraPaletteInfo.size(); z++) {

			ExtraPalette epp = tile.extraPaletteInfo[z];

			// Create a new tile with the given palette
			Tile extraTile(assetData->image.tile_h * assetData->image.tile_w);
			extraTile.pal = epp.palette;

			// Copy over the pixel data from the extra palette struct
			for(int b = 0; b < epp.pixelData.size(); b++) {
				extraTile.data[b] = (epp.pixelData[b]);
			}

			if(assetData->args->keep_duplicate_tiles)
			{
				assetData->tiles.push_back(extraTile);
				idx = assetData->tiles.size() - 1;
				props = assetData->args->props_default;
			}
			else
			{
				if(!FindTile(extraTile, idx, props,assetData))
				{
					if(assetData->args->source_tilesets.size()>0) {
						printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, assetData->args->extra_tile_count + 1);
						assetData->args->extra_tile_count++;
						assetData->args->includeTileData = true;
					}
					assetData->tiles.push_back(extraTile);
					idx = assetData->tiles.size() - 1;
					props = assetData->args->props_default;
				}
			}

			props |= epp.palette;

			// Scale up index based on 8x8 tiles-per-hardware sprite
			if(assetData->args->sprite_mode == SPR_8x16)
				idx *= 2;
			else if(assetData->args->sprite_mode == SPR_16x16_MSX)
				idx *= 4;

			mt_sprite.push_back(MTTile(x - *last_x, y - *last_y, (unsigned char)idx, props));

			*layeredSpritesCount = (*layeredSpritesCount)+1;

			*last_x = x;
			*last_y = y;
		}
	}

}

bool GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y, PNG2AssetData* assetData)
{
	int last_x = _x + pivot_x;
	int last_y = _y + pivot_y;

	assetData->sprites.push_back(MetaSprite());
	MetaSprite& mt_sprite = assetData->sprites.back();
	for(int y = _y; y < _y + _h && y < (int)assetData->image.h; y += assetData->image.tile_h)
	{
		int layeredSpritesCount = 0;
		int nonLayeredSpritesCount = 0;

		for(int x = _x; x < _x + _w && x < (int)assetData->image.w; x += assetData->image.tile_w)
		{
			Tile tile(assetData->image.tile_h * assetData->image.tile_w);
			if(assetData->image.ExtractTile(x, y, tile, assetData->args->sprite_mode, assetData->args->export_as_map, assetData->args->use_map_attributes))
			{
				size_t idx;
				unsigned char props;

				int old_last_x = last_x;
				int old_last_y = last_y;

				if(assetData->args->keep_palette_order) {
					GetMetaspriteLayeredTiles(tile, mt_sprite, &last_x, &last_y, x, y,assetData,&layeredSpritesCount);
				}

				last_x = old_last_x;
				last_y = old_last_y;

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

				nonLayeredSpritesCount++;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
				last_x = x;
				last_y = y;
			}
		}

		int currentRow = y / assetData->image.tile_h;

		int max = 10;

		// As Per: https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_supported_consoles.html
		// GB, AP, Duck all support 10 sprites per line
		// SMS, and GG support 8 per line.
		if(assetData->args->pack_mode == Tile::SMS|| assetData->args->pack_mode == Tile::NES){
			max = 8;
		}

		int totalLayeredSprites = layeredSpritesCount + nonLayeredSpritesCount;

		// If we've exceeded the maxium amount
		if(totalLayeredSprites > max) {
			printf("\n//////////////////////////////////////////////////////////////////////////////////////////////////////////\n");
			printf("\ERROR: You have %d total sprites on row %d! This is %d above the maximum of %d. Not All sprites will be shown!\n", totalLayeredSprites, currentRow,totalLayeredSprites-max,max);
			printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
			printf("Ending the process early!\n\n");

			// Return false so the process ends early
			return false;

		// If we're at the maximum amount
		}else if(totalLayeredSprites == max) {

			printf("\n//////////////////////////////////////////////////////////////////////////////////////////////////////////\n");
			printf("\nWARNING: You have %d total sprites on row %d! This is the maximum amount your hardware can support. No other sprites will be drawn that share a scanline (wording?)\n", totalLayeredSprites, currentRow);
			printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////\n");
		}

		else if(layeredSpritesCount > 0) {

			printf("\nYou have %d extra layered sprites on row: %d. Making a total of %d sprites.\n\n", layeredSpritesCount, currentRow, totalLayeredSprites);
			printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////\n");
			printf("WARNING: There are hardware limits on how many sprites can be drawn per scanline. Take that into consideration when using heavily-layered metasprites.\n");
			printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
		}
	}

	return true;

}


bool GetAllMetasprites(PNG2AssetData* assetData) {

	//Extract metasprites
	for(int y = 0; y < (int)assetData->image.h; y += (unsigned int)assetData->args->spriteSize.height)
	{
		for(int x = 0; x < (int)assetData->image.w; x += (unsigned int)assetData->args->spriteSize.width)
		{
			// Check the response, true means everything's ok, false means too many sprites per scanline
			if(!GetMetaSprite(x, y, (unsigned int)assetData->args->spriteSize.width, (unsigned int)assetData->args->spriteSize.height, assetData->args->pivot.x, assetData->args->pivot.y, assetData)) {

				// False means we've got too many sprites per scanline
				return false;
			}
		}
	}

	return true;
}
