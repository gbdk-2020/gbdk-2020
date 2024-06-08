#pragma once


#include <vector>
#include <string>
#include "vector2.h"
#include "mttile.h"
#include "tiles.h"
#include "metasprites.h"
#include "cmp_int_color.h"

using namespace std;


struct PNG2AssetArguments {

    Vector2Size spriteSize;
    Vector2Size map_attributes_size;
    Vector2Size map_attributes_packed_size;

    Rectangle pivot;

    //default values for some params

    string output_filename_h;
    string output_filename_bin;
    string output_filename_attributes_bin;
    string output_filename_tiles_bin;
    string data_name;
    string output_filename;
    string input_filename;
    vector<string> source_tilesets;
    string entity_tileset_filename;

    size_t max_palettes;

    bool keep_palette_order;
    bool repair_indexed_pal;
    bool output_binary;
    bool output_transposed;
    bool export_as_map;
    bool use_map_attributes;
    bool use_2x2_map_attributes;
    bool pack_map_attributes;
    bool convert_rgb_to_nes;
    bool includeTileData;
    bool includedMapOrMetaspriteData;
    bool keep_duplicate_tiles;
    bool keep_empty_sprite_tiles;
    bool include_palettes;
    bool use_structs;
    bool flip_tiles;
    bool relative_paths;

    int errorCode;
    int bank;
    int sprite_mode;
    int bpp;
    int props_default;  // Default Sprite props has no attributes enabled

    unsigned int tile_origin; // Default to no tile index offset
    size_t source_total_color_count;  // Total number of colors (palette_count x colors_per_palette)
    unsigned int source_tileset_size;
    bool has_source_tilesets;
    bool has_entity_tileset;
    int processing_mode;  // Whether the current image being processed is a source tileset (MODE_SOURCE_TILESET), entity_tileset (MODE_ENTITY_TILESET), or the main image (MODE_MAIN_IMAGE)

    Tile::PackMode pack_mode;
    int map_entry_size_bytes;

};


int processPNG2AssetArguments(int argc, char* argv[], PNG2AssetArguments* args);