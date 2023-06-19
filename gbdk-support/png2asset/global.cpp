#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "global.h"
#include "mttile.h"

// TODO: Moved these vars to global scope (from giant main()) as start of breaking main into functions
//       They should get encapsulated
string output_filename_h;
string output_filename_bin;
string output_filename_attributes_bin;
string output_filename_tiles_bin;
string data_name;
//default values for some params
int  sprite_w = 0;
int  sprite_h = 0;
int  pivot_x = 0xFFFFFF;
int  pivot_y = 0xFFFFFF;
int  pivot_w = 0xFFFFFF;
int  pivot_h = 0xFFFFFF;
string output_filename;
int  bank = -1;
bool keep_palette_order = false;
bool repair_indexed_pal = false;
bool output_binary = false;
bool output_transposed = false;
size_t max_palettes = 8;

bool export_as_map = false;
bool use_map_attributes = false;
bool use_2x2_map_attributes = false;
bool pack_map_attributes = false;
bool convert_rgb_to_nes = false;
size_t map_attributes_width = 0;
size_t map_attributes_height = 0;
size_t map_attributes_packed_width = 0;
size_t map_attributes_packed_height = 0;
int sprite_mode = SPR_8x16;

int bpp = 2;
unsigned int tile_origin = 0; // Default to no tile index offset
string source_tileset;
unsigned int extra_tile_count = 0;
unsigned int source_total_color_count = 0;  // Total number of colors (palette_count x colors_per_palette)
unsigned int source_tileset_size = 0;
bool includeTileData = true;
bool includedMapOrMetaspriteData = true;
PNGImage source_tileset_image;
bool use_source_tileset = false;
bool keep_duplicate_tiles = false;
bool include_palettes = true;

vector< Tile > tiles;
vector<	MetaSprite > sprites;
vector< unsigned char > map;
vector< unsigned char > map_attributes;
PNGImage image;
int props_default = 0x00;  // Default Sprite props has no attributes enabled
bool use_structs = false;
bool flip_tiles = true;
Tile::PackMode pack_mode = Tile::GB;