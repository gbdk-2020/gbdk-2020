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

int decodePNG(vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
void loadFile(vector<unsigned char>& buffer, const std::string& filename);


int main(int argc, char* argv[])
{
	if (argc < 2)
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
	output_filename = argv[1];
	output_filename = output_filename.substr(0, output_filename.size() - 4) + ".c";

	//Parse argv
	for(int i = 2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-sw"))
		{
			sprite_w = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-sh"))
		{
			sprite_h = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-sp"))
		{
			props_default = strtol(argv[++i], NULL, 16);
		}
		if(!strcmp(argv[i], "-px"))
		{
			pivot_x = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-py"))
		{
			pivot_y = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-pw"))
		{
			pivot_w = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-ph"))
		{
			pivot_h = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-spr8x8"))
		{
			image.tile_w = 8;
			image.tile_h = 8;
			sprite_mode = SPR_8x8;
		}
		else if(!strcmp(argv[i],"-spr8x16"))
		{
			image.tile_w = 8;
			image.tile_h = 16;
			sprite_mode = SPR_8x16;
		}
		else if(!strcmp(argv[i],"-spr16x16msx"))
		{
			image.tile_w = 16;
			image.tile_h = 16;
			sprite_mode = SPR_16x16_MSX;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			output_filename = argv[++ i];
		}
		else if(!strcmp(argv[i], "-b"))
		{
			bank = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-keep_palette_order"))
		{
			keep_palette_order = true;
		}
		else if(!strcmp(argv[i], "-repair_indexed_pal"))
		{
			repair_indexed_pal = true;
			keep_palette_order = true; // -repair_indexed_pal requires -keep_palette_order, so force it on
		}
		else if(!strcmp(argv[i], "-noflip"))
		{
			flip_tiles = false;
		}
		else if(!strcmp(argv[i], "-map"))
		{
			export_as_map = true;
		}
		else if(!strcmp(argv[i], "-use_map_attributes"))
		{
			use_map_attributes = true;
		}
		else if (!strcmp(argv[i], "-use_nes_attributes"))
		{
			use_map_attributes = true;
			use_2x2_map_attributes = true;
			pack_map_attributes = true;
		}
		else if (!strcmp(argv[i], "-use_nes_colors"))
		{
			convert_rgb_to_nes = true;
		}
		else if(!strcmp(argv[i], "-use_structs"))
		{
			use_structs = true;
		}
		else if(!strcmp(argv[i], "-bpp"))
		{
			bpp = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-max_palettes"))
		{
			max_palettes = atoi(argv[++ i]);
			if (max_palettes == 0)
			{
				printf("-max_palettes must be larger than zero\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-pack_mode"))
		{
			std::string pack_mode_str = argv[++ i];
			if     (pack_mode_str == "gb")  pack_mode = Tile::GB;
			else if(pack_mode_str == "sgb") pack_mode = Tile::SGB;
			else if(pack_mode_str == "sms") pack_mode = Tile::SMS;
			else if(pack_mode_str == "1bpp") pack_mode = Tile::BPP1;
			else
			{
				printf("-pack_mode must be one of gb, sgb, sms, 1bpp\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-tile_origin"))
		{
			tile_origin = atoi(argv[++ i]);
		}
		else if (!strcmp(argv[i], "-maps_only") || !strcmp(argv[i], "-metasprites_only"))
		{
			includeTileData = false;
		}
		else if (!strcmp(argv[i], "-tiles_only"))
		{
			includedMapOrMetaspriteData = false;
		}
		else if (!strcmp(argv[i], "-keep_duplicate_tiles"))
		{
			keep_duplicate_tiles = true;
		}
		else if (!strcmp(argv[i], "-no_palettes"))
		{
			include_palettes = false;
		}
		else if (!strcmp(argv[i], "-source_tileset"))
		{
			use_source_tileset = true;
			includeTileData = false;
			source_tileset = argv[++i];
		}
		else if (!strcmp(argv[i], "-bin"))
		{
			output_binary = true;
		}
		else if (!strcmp(argv[i], "-transposed"))
		{
			output_transposed = true;
		}
	}

	image.colors_per_pal = 1 << bpp;

	if(export_as_map)
	{
		image.tile_w = 8; //Force tiles_w to 8 on maps
		image.tile_h = 8; //Force tiles_h to 8 on maps
		sprite_mode = SPR_NONE;
	}

	int slash_pos = (int)output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)output_filename.find_last_of('\\');
	int dot_pos = (int)output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

	output_filename_h = output_filename.substr(0, dot_pos) + ".h";
	output_filename_bin = output_filename.substr(0, dot_pos) + "_map.bin";
	output_filename_attributes_bin = output_filename.substr(0, dot_pos) + "_map_attributes.bin";
	output_filename_tiles_bin = output_filename.substr(0, dot_pos) + "_tiles.bin";
	data_name = output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(data_name.begin(), data_name.end(), '-', '_');

	// This was moved from outside the upcoming else statement when not using keep_palette_order
	// So the 'GetSourceTileset' function can pre-populate it from the source tileset
	vector< SetPal > palettes;

	// Copy some settings into optional source tileset image
	source_tileset_image.colors_per_pal = image.colors_per_pal;
	source_tileset_image.tile_w = image.tile_w;
	source_tileset_image.tile_h = image.tile_h;

	if (use_source_tileset) {

		if (!GetSourceTileset(repair_indexed_pal, keep_palette_order, max_palettes, palettes)) {
			return 1;
		}
	}


	//load and decode png
	vector<unsigned char> buffer;
	lodepng::load_file(buffer, argv[1]);
	lodepng::State state;

	if (keep_palette_order) {
		//Calling with keep_palette_order means
		//-The image should be png indexed (1-8 bits per pixel)
		//-For CGB: Each 4 colors define a gbc palette, the first color is the transparent one
		//-Each rectangle with dimension(tile_w, tile_h) in the image has colors from one of those palettes only
		state.info_raw.colortype = LCT_PALETTE;
		state.info_raw.bitdepth = 8;

		// * Do *NOT* turn color_convert ON here.
		// When source PNG is indexed with bit depth was less than 8 bits per pixel then
		// color_convert may mangle the packed indexed values. Instead manually unpack them.
		//
		// This is necessary since some PNG encoders will use the minimum possible number of bits.
		//   For example 2 colors in the palette -> 1bpp -> 8 pixels per byte in the decoded image.
		//     Also see below about requirement to use palette from source image
		state.decoder.color_convert = false;

		unsigned error = lodepng::decode(image.data, image.w, image.h, state, buffer);
		// Unpack the image if needed. Also checks and errors on incompatible palette type if needed
		if (! image_indexed_ensure_8bpp(image.data, image.w, image.h, (int)state.info_png.color.bitdepth, (int)state.info_png.color.colortype))
			return 1;
		else if(error) {
			printf("decoder error %s\n", lodepng_error_text(error));
			return 1;
		}

		// Use source image palette since lodepng conversion to indexed (LCT_PALETTE) won't create a palette
		// So: state.info_png.color.palette/size instead of state.info_raw.palette/size
		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, state.info_png.color.palettesize / image.colors_per_pal);
		image.total_color_count = palette_count * image.colors_per_pal;
		image.palette = state.info_png.color.palette;


		if (repair_indexed_pal)
			if (!image_indexed_repair_tile_palettes(image, use_2x2_map_attributes))
				return 1;

		// TODO: Enable dimension check
		// // Validate image dimensions
		// if (((image.w % tile_w) != 0) || ((image.h % tile_h) != 0))
		// {
		// 	printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image.w, image.h, tile_w, tile_h);
		// 	return 1;
		// }

		if (use_source_tileset) {

			// Make sure these two values match when keeping palette order
			if (image.total_color_count != source_tileset_image.total_color_count) {

				printf("error: The number of color palette's for your source tileset (%d) and target image (%d) do not match.", (unsigned int)source_tileset_image.total_color_count, (unsigned int)image.total_color_count);
				return 1;
			}

			size_t size = max(image.total_color_count, source_tileset_image.total_color_count);

			// Make sure these two values match when keeping palette order
			if (memcmp(image.palette, source_tileset_image.palette, size) != 0) {

				printf("error: The palette's for your source tileset and target image do not match.");
				return 1;
			}
		}
	}
	else
	{
		PNGImage image32;
		image32.colors_per_pal = image.colors_per_pal;
		image32.tile_w = image.tile_w;
		image32.tile_h = image.tile_h;

		unsigned error = lodepng::decode(image32.data, image32.w, image32.h, state, buffer); //decode as 32 bit
		if(error)
		{
			printf("decoder error %s\n", lodepng_error_text(error));
			return 1;
		}

		// Validate image dimensions
		if( ((image32.w % image32.tile_w) != 0) || ((image32.h % image32.tile_h) != 0) )
		{
			printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image32.w, image32.h, image32.tile_w, image32.tile_h);
			return 1;
		}

		int* palettes_per_tile = BuildPalettesAndAttributes(image32, palettes, use_2x2_map_attributes);

		//Create the indexed image
		image.data.clear();
		image.w = image32.w;
		image.h = image32.h;

		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, palettes.size());

		image.total_color_count = palette_count * image.colors_per_pal;
		image.palette = new unsigned char[palette_count * image.colors_per_pal * RGBA32_SZ]; // total color count * 4 bytes each

		// If we are using a sourcetileset and have more palettes than it defines
		if (use_source_tileset && (image.total_color_count > source_total_color_count)) {
			printf("Found %d extra palette(s) for target tilemap.\n", (unsigned int)((image.total_color_count - source_total_color_count) / image.colors_per_pal));
		}
		for(size_t p = 0; p < palette_count; ++p)
		{
			int *color_ptr = (int*)&image.palette[p * image.colors_per_pal * RGBA32_SZ];

			//TODO: if palettes[p].size() != image.colors_per_pal we should probably try to fill the gaps based on grayscale values

			for(SetPal::iterator it = palettes[p].begin(); it != palettes[p].end(); ++ it, color_ptr ++)
			{
				unsigned char* c = (unsigned char*)&(*it);
				*color_ptr = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
			}
		}

		for(size_t y = 0; y < image32.h; ++ y)
		{
			for(size_t x = 0; x < image32.w; ++x)
			{
				unsigned char* c32ptr = &image32.data[(image32.w * y + x) * RGBA32_SZ];
				int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
				unsigned char palette = palettes_per_tile[(y / image32.tile_h) * (image32.w / image32.tile_w) + (x / image32.tile_w)];
				unsigned char index = std::distance(palettes[palette].begin(), palettes[palette].find(color32));
				image.data.push_back((palette << bpp) + index);
			}
		}

		//Test: output png to see how it looks
		//Export(image, "temp.png");
	}

	if(sprite_w == 0) sprite_w = (int)image.w;
	if(sprite_h == 0) sprite_h = (int)image.h;
	if(pivot_x == 0xFFFFFF) pivot_x = sprite_w / 2;
	if(pivot_y == 0xFFFFFF) pivot_y = sprite_h / 2;
	if(pivot_w == 0xFFFFFF) pivot_w = sprite_w;
	if(pivot_h == 0xFFFFFF) pivot_h = sprite_h;

	if(export_as_map)
	{
		//Extract map
		GetMap();
	}
	else
	{
		//Extract metasprites
		for(int y = 0; y < (int)image.h; y += sprite_h)
		{
			for(int x = 0; x < (int)image.w; x += sprite_w)
			{
				GetMetaSprite(x, y, sprite_w, sprite_h, pivot_x, pivot_y);
			}
		}
	}
	map_attributes_width = image.w / 8;
	map_attributes_height = image.h / 8;
	// Optionally perform 2x2 reduction on attributes (NES attribute table has this format)
	if(use_2x2_map_attributes)
	{
		// NES attribute map dimensions are half-resolution 
		ReduceMapAttributes2x2(palettes);
	}
	// Optionally align and pack map attributes into NES PPU format
	if (pack_map_attributes)
	{
		AlignMapAttributes();
		PackMapAttributes();
	}
	else
	{
		// Use original attribute dimensions for packed
		map_attributes_packed_width = map_attributes_width;
		map_attributes_packed_height = map_attributes_height;
	}

	// === EXPORT ===

	// Header file export
	if (export_h_file() == false) return 1; // Exit with Fail

	if ((export_as_map) && (output_binary)) {
		// Handle special case of binary map export
		export_map_binary();
	} else {
		// Normal source file export
		if (export_c_file() == false) return 1; // Exit with Fail
	}
}
