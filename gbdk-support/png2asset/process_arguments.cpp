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
#include "map_attributes.h"
#include "palettes.h"
#include "source_tileset.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "image_data.h"
#include "maps.h"
#include "metasprites.h"
#include "metasprites_functions.h"
#include "process_arguments.h"



int PNG2AssetData::Execute() {

	// if we are using a source tileset
	if(this->arguments.use_source_tileset) {

		// Handle generation of the source tileset
		int handleSourceTilesetValue = HandleSourceTileset(this);

		// Return the error code if the function returns non-zero
		if(handleSourceTilesetValue != 0) {
			return handleSourceTilesetValue;
		}
	}

	int readImageDataValue = ReadImageData(this);

	// Return the error code if the function returns non-zero
	if(readImageDataValue != 0) {
		return readImageDataValue;
	}

	// Get the data depending on the type
	if(this->arguments.export_as_map)GetMap(this);
	else GetAllMetasprites(this);

	return HandleExport(this);
}

PNG2AssetData::PNG2AssetData(int argc, char* argv[]) {

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
		errorCode = 0;
		return;
	}

	//default params
	this->arguments.input_filename = argv[1];
	this->arguments.output_filename = argv[1];
	this->arguments.output_filename = this->arguments.output_filename.substr(0, this->arguments.output_filename.size() - 4) + ".c";

	//Parse argv
	for(int i = 2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-sw"))
		{
			this->arguments.sprite_w = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-sh"))
		{
			this->arguments.sprite_h = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-sp"))
		{
			this->arguments.props_default = strtol(argv[++i], NULL, 16);
		}
		if(!strcmp(argv[i], "-px"))
		{
			this->arguments.pivot_x = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-py"))
		{
			this->arguments.pivot_y = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-pw"))
		{
			this->arguments.pivot_w = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-ph"))
		{
			this->arguments.pivot_h = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-spr8x8"))
		{
			this->image.tile_w = 8;
			this->image.tile_h = 8;
			this->arguments.sprite_mode = SPR_8x8;
		}
		else if(!strcmp(argv[i], "-spr8x16"))
		{
			this->image.tile_w = 8;
			this->image.tile_h = 16;
			this->arguments.sprite_mode = SPR_8x16;
		}
		else if(!strcmp(argv[i], "-spr16x16msx"))
		{
			this->image.tile_w = 16;
			this->image.tile_h = 16;
			this->arguments.sprite_mode = SPR_16x16_MSX;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			this->arguments.output_filename = argv[++i];
		}
		else if(!strcmp(argv[i], "-b"))
		{
			this->arguments.bank = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-keep_palette_order"))
		{
			this->arguments.keep_palette_order = true;
		}
		else if(!strcmp(argv[i], "-repair_indexed_pal"))
		{
			this->arguments.repair_indexed_pal = true;
			this->arguments.keep_palette_order = true; // -repair_indexed_pal requires -keep_palette_order, so force it on
		}
		else if(!strcmp(argv[i], "-noflip"))
		{
			this->arguments.flip_tiles = false;
		}
		else if(!strcmp(argv[i], "-map"))
		{
			this->arguments.export_as_map = true;
		}
		else if(!strcmp(argv[i], "-use_map_attributes"))
		{
			this->arguments.use_map_attributes = true;
		}
		else if(!strcmp(argv[i], "-use_nes_attributes"))
		{
			this->arguments.use_map_attributes = true;
			this->arguments.use_2x2_map_attributes = true;
			this->arguments.pack_map_attributes = true;
		}
		else if(!strcmp(argv[i], "-use_nes_colors"))
		{
			this->arguments.convert_rgb_to_nes = true;
		}
		else if(!strcmp(argv[i], "-use_structs"))
		{
			this->arguments.use_structs = true;
		}
		else if(!strcmp(argv[i], "-bpp"))
		{
			this->arguments.bpp = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-max_palettes"))
		{
			this->arguments.max_palettes = atoi(argv[++i]);
			if(this->arguments.max_palettes == 0)
			{
				printf("-max_palettes must be larger than zero\n");
				this->errorCode = 1;
				return;
			}
		}
		else if(!strcmp(argv[i], "-pack_mode"))
		{
			std::string pack_mode_str = argv[++i];
			if(pack_mode_str == "gb")  this->arguments.pack_mode = Tile::GB;
			else if(pack_mode_str == "sgb") this->arguments.pack_mode = Tile::SGB;
			else if(pack_mode_str == "sms") this->arguments.pack_mode = Tile::SMS;
			else if(pack_mode_str == "1bpp") this->arguments.pack_mode = Tile::BPP1;
			else
			{
				printf("-pack_mode must be one of gb, sgb, sms, 1bpp\n");
				this->errorCode = 1;
				return;
			}
		}
		else if(!strcmp(argv[i], "-tile_origin"))
		{
			this->arguments.tile_origin = atoi(argv[++i]);
		}
		else if(!strcmp(argv[i], "-maps_only") || !strcmp(argv[i], "-metasprites_only"))
		{
			this->arguments.includeTileData = false;
		}
		else if(!strcmp(argv[i], "-tiles_only"))
		{
			this->arguments.includedMapOrMetaspriteData = false;
		}
		else if(!strcmp(argv[i], "-keep_duplicate_tiles"))
		{
			this->arguments.keep_duplicate_tiles = true;
		}
		else if(!strcmp(argv[i], "-no_palettes"))
		{
			this->arguments.include_palettes = false;
		}
		else if(!strcmp(argv[i], "-source_tileset"))
		{
			this->arguments.use_source_tileset = true;
			this->arguments.includeTileData = false;
			this->arguments.source_tileset = argv[++i];
		}
		else if(!strcmp(argv[i], "-bin"))
		{
			this->arguments.output_binary = true;
		}
		else if(!strcmp(argv[i], "-transposed"))
		{
			this->arguments.output_transposed = true;
		}
	}


	int slash_pos = (int)this->arguments.output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)this->arguments.output_filename.find_last_of('\\');
	int dot_pos = (int)this->arguments.output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

	this->arguments.output_filename_h = this->arguments.output_filename.substr(0, dot_pos) + ".h";
	this->arguments.output_filename_bin = this->arguments.output_filename.substr(0, dot_pos) + "_map.bin";
	this->arguments.output_filename_attributes_bin = this->arguments.output_filename.substr(0, dot_pos) + "_map_attributes.bin";
	this->arguments.output_filename_tiles_bin = this->arguments.output_filename.substr(0, dot_pos) + "_tiles.bin";
	this->arguments.data_name = this->arguments.output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(this->arguments.data_name.begin(), this->arguments.data_name.end(), '-', '_');

	this->errorCode = 0;
}