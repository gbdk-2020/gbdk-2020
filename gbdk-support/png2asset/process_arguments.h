#pragma once


#include <vector>
#include <string>
#include "mttile.h"
#include "metasprites.h"
#include "cmp_int_color.h"
#include "png2asset.h"
using namespace std;

class PNG2AssetData {

public:

	string output_filename_h;
	string output_filename_bin;
	string output_filename_attributes_bin;
	string output_filename_tiles_bin;
	string data_name;
	//default values for some params
	int  sprite_w;
	int  sprite_h;
	int  pivot_x;
	int  pivot_y;
	int  pivot_w;
	int  pivot_h;
	string output_filename;
	string input_filename;
	int  bank;
	bool keep_palette_order;
	bool repair_indexed_pal;
	bool output_binary;
	bool output_transposed;
	size_t max_palettes;

	bool export_as_map;
	bool use_map_attributes;
	bool use_2x2_map_attributes;
	bool pack_map_attributes;
	bool convert_rgb_to_nes;
	size_t map_attributes_width;
	size_t map_attributes_height;
	size_t map_attributes_packed_width;
	size_t map_attributes_packed_height;
	int sprite_mode;

	int bpp;
	unsigned int tile_origin; // Default to no tile index offset
	string source_tileset;
	unsigned int extra_tile_count;
	unsigned int source_total_color_count;  // Total number of colors (palette_count x colors_per_palette)
	unsigned int source_tileset_size;
	bool includeTileData;
	bool includedMapOrMetaspriteData;
	PNGImage source_tileset_image;
	bool use_source_tileset;
	bool keep_duplicate_tiles;
	bool include_palettes;

	vector< Tile > tiles;
	vector<	MetaSprite > sprites;
	vector< unsigned char > map;
	vector< unsigned char > map_attributes;
	PNGImage image;
	int props_default;  // Default Sprite props has no attributes enabled
	bool use_structs;
	bool flip_tiles;
	Tile::PackMode pack_mode;

	unsigned char rgb_to_nes[64];
};

int ProcessArguments(int argc, char* argv[], PNG2AssetData* png2AssetData);