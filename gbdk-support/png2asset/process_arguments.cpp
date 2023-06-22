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
#include "map_attributes.h"
#include "palettes.h"
#include "source_tileset.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"
#include "process_arguments.h"

int ProcessArguments(int argc, char* argv[], PNG2AssetData* png2AssetData) {

	if(argc < 2)
	{
		printf("usage: png2asset    <file>.png [options]\n");
		printf("-c                  ouput file (default: <png file>.c)\n");
		printf("-sw <width>         metasprites width size (default: png width)\n");
		printf("-sh <height>        metasprites height size (default: png height)\n");
		printf("-sp <props>         change default for sprite OAM property bytes (in hex) (default: 0x00)\n");
		printf("-px <x coord>       metasprites pivot x coordinate (default: metasprites width / 2)\n");
		printf("-py <y coord>       metasprites pivot y coordinate (default: metasprites height / 2)\n");
		printf("-pw <width>         metasprites collision rect width (default: metasprites width)\n");
		printf("-ph <height>        metasprites collision rect height (default: metasprites height)\n");
		printf("-spr8x8             use SPRITES_8x8\n");
		printf("-spr8x16            use SPRITES_8x16 (this is the default)\n");
		printf("-spr16x16msx        use SPRITES_16x16\n");
		printf("-b <bank>           bank (default: fixed bank)\n");
		printf("-keep_palette_order use png palette\n");
		printf("-repair_indexed_pal try to repair indexed tile palettes (implies \"-keep_palette_order\")\n");
		printf("-noflip             disable tile flip\n");
		printf("-map                Export as map (tileset + bg)\n");
		printf("-use_map_attributes Use CGB BG Map attributes\n");
		printf("-use_nes_attributes Use NES BG Map attributes\n");
		printf("-use_nes_colors     Convert RGB color values to NES PPU colors\n");
		printf("-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)\n");
		printf("-bpp                bits per pixel: 1, 2, 4 (default: 2)\n");
		printf("-max_palettes       max number of palettes allowed (default: 8)\n");
		printf("                    (note: max colors = max_palettes x num colors per palette)\n");
		printf("-pack_mode          gb, sgb, sms, 1bpp (default: gb)\n");
		printf("-tile_origin        tile index offset for maps (default: 0)\n");

		printf("-tiles_only         export tile data only\n");
		printf("-maps_only          export map tilemap only\n");
		printf("-metasprites_only   export metasprite descriptors only\n");
		printf("-source_tileset     use source tileset (image with common tiles)\n");
		printf("-keep_duplicate_tiles   do not remove duplicate tiles (default: not enabled)\n");
		printf("-no_palettes        do not export palette data\n");

		printf("-bin                export to binary format\n");
		printf("-transposed         export transposed (column-by-column instead of row-by-row)\n");
		return 0;
	}

	//default params
	png2AssetData->input_filename = argv[1];
	png2AssetData->output_filename = argv[1];
	png2AssetData->output_filename = png2AssetData->output_filename.substr(0, png2AssetData->output_filename.size() - 4) + ".c";

	//Parse argv
	for(int i = 2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-sw"))
		{
			png2AssetData->sprite_w = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-sh"))
		{
			png2AssetData->sprite_h = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-sp"))
		{
			png2AssetData->props_default = strtol(argv[++i], NULL, 16);
		}
		if(!strcmp(argv[i], "-px"))
		{
			png2AssetData->pivot_x = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-py"))
		{
			png2AssetData->pivot_y = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-pw"))
		{
			png2AssetData->pivot_w = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-ph"))
		{
			png2AssetData->pivot_h = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-spr8x8"))
		{
			png2AssetData->image.tile_w = 8;
			png2AssetData->image.tile_h = 8;
			png2AssetData->sprite_mode = SPR_8x8;
		}
		else if(!strcmp(argv[i], "-spr8x16"))
		{
			png2AssetData->image.tile_w = 8;
			png2AssetData->image.tile_h = 16;
			png2AssetData->sprite_mode = SPR_8x16;
		}
		else if(!strcmp(argv[i], "-spr16x16msx"))
		{
			png2AssetData->image.tile_w = 16;
			png2AssetData->image.tile_h = 16;
			png2AssetData->sprite_mode = SPR_16x16_MSX;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			png2AssetData->output_filename = argv[++i];
		}
		else if(!strcmp(argv[i], "-b"))
		{
			png2AssetData->bank = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-keep_palette_order"))
		{
			png2AssetData->keep_palette_order = true;
		}
		else if(!strcmp(argv[i], "-repair_indexed_pal"))
		{
			png2AssetData->repair_indexed_pal = true;
			png2AssetData->keep_palette_order = true; // -repair_indexed_pal requires -keep_palette_order, so force it on
		}
		else if(!strcmp(argv[i], "-noflip"))
		{
			png2AssetData->flip_tiles = false;
		}
		else if(!strcmp(argv[i], "-map"))
		{
			png2AssetData->export_as_map = true;
		}
		else if(!strcmp(argv[i], "-use_map_attributes"))
		{
			png2AssetData->use_map_attributes = true;
		}
		else if(!strcmp(argv[i], "-use_nes_attributes"))
		{
			png2AssetData->use_map_attributes = true;
			png2AssetData->use_2x2_map_attributes = true;
			png2AssetData->pack_map_attributes = true;
		}
		else if(!strcmp(argv[i], "-use_nes_colors"))
		{
			png2AssetData->convert_rgb_to_nes = true;
		}
		else if(!strcmp(argv[i], "-use_structs"))
		{
			png2AssetData->use_structs = true;
		}
		else if(!strcmp(argv[i], "-bpp"))
		{
			png2AssetData->bpp = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-max_palettes"))
		{
			png2AssetData->max_palettes = atoi(argv[++i]);
			if(png2AssetData->max_palettes == 0)
			{
				printf("-max_palettes must be larger than zero\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-pack_mode"))
		{
			std::string pack_mode_str = argv[++i];
			if(pack_mode_str == "gb")  png2AssetData->pack_mode = Tile::GB;
			else if(pack_mode_str == "sgb") png2AssetData->pack_mode = Tile::SGB;
			else if(pack_mode_str == "sms") png2AssetData->pack_mode = Tile::SMS;
			else if(pack_mode_str == "1bpp") png2AssetData->pack_mode = Tile::BPP1;
			else
			{
				printf("-pack_mode must be one of gb, sgb, sms, 1bpp\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-tile_origin"))
		{
			png2AssetData->tile_origin = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-maps_only") || !strcmp(argv[i], "-metasprites_only"))
		{
			png2AssetData->includeTileData = false;
		}
		else if(!strcmp(argv[i], "-tiles_only"))
		{
			png2AssetData->includedMapOrMetaspriteData = false;
		}
		else if(!strcmp(argv[i], "-keep_duplicate_tiles"))
		{
			png2AssetData->keep_duplicate_tiles = true;
		}
		else if(!strcmp(argv[i], "-no_palettes"))
		{
			png2AssetData->include_palettes = false;
		}
		else if(!strcmp(argv[i], "-source_tileset"))
		{
			png2AssetData->use_source_tileset = true;
			png2AssetData->includeTileData = false;
			png2AssetData->source_tileset = argv[++i];
		}
		else if(!strcmp(argv[i], "-bin"))
		{
			png2AssetData->output_binary = true;
		}
		else if(!strcmp(argv[i], "-transposed"))
		{
			png2AssetData->output_transposed = true;
		}
	}


	int slash_pos = (int)png2AssetData->output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)png2AssetData->output_filename.find_last_of('\\');
	int dot_pos = (int)png2AssetData->output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

	png2AssetData->output_filename_h = png2AssetData->output_filename.substr(0, dot_pos) + ".h";
	png2AssetData->output_filename_bin = png2AssetData->output_filename.substr(0, dot_pos) + "_map.bin";
	png2AssetData->output_filename_attributes_bin = png2AssetData->output_filename.substr(0, dot_pos) + "_map_attributes.bin";
	png2AssetData->output_filename_tiles_bin = png2AssetData->output_filename.substr(0, dot_pos) + "_tiles.bin";
	png2AssetData->data_name = png2AssetData->output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(png2AssetData->data_name.begin(), png2AssetData->data_name.end(), '-', '_');

	return 0;
}