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

#include "cmp_int_color.h"

#include "image_utils.h"
#include "image_data.h"
#include "maps.h"
#include "metasprites.h"
#include "metasprites_functions.h"
#include "process_arguments.h"
#include "png_image.h"
#include "tiles.h"


using namespace std;

int processPNG2AssetArguments(int argc, char* argv[], PNG2AssetArguments* args) {

    //default values for some params
    args->spriteSize.width = 0;
    args->spriteSize.height = 0;
    args->map_attributes_size.width = 0;
    args->map_attributes_size.height = 0;
    args->map_attributes_packed_size.width = 0;
    args->map_attributes_packed_size.height = 0;

    args->pivot.x = 0xFFFFFF;
    args->pivot.y = 0xFFFFFF;
    args->pivot.width = 0xFFFFFF;
    args->pivot.height = 0xFFFFFF;


    args->max_palettes = 8;

    args->keep_palette_order = false;
    args->repair_indexed_pal = false;
    args->output_binary = false;
    args->output_transposed = false;
    args->export_as_map = false;
    args->use_map_attributes = false;
    args->use_2x2_map_attributes = false;
    args->pack_map_attributes = false;
    args->convert_rgb_to_nes = false;
    args->includeTileData = true;
    args->includedMapOrMetaspriteData = true;
    args->keep_duplicate_tiles = false;
    args->keep_empty_sprite_tiles = false;
    args->include_palettes = true;
    args->use_structs = false;
    args->flip_tiles = true;

    // args->errorCode;
    args->bank = -1;
    args->sprite_mode = SPR_8x16;
    args->bpp = 2;
    args->props_default = 0;

    args->tile_origin = 0; // Default to no tile index offset
    args->source_total_color_count = 0;
    args->source_tileset_size = 0;
    args->has_source_tilesets = false;

    args->has_entity_tileset = false;

    args->processing_mode = MODE_MAIN_IMAGE;

    args->pack_mode = Tile::GB;
    args->map_entry_size_bytes = 1;

    args->relative_paths = false;

    if(argc < 2)
    {
        printf("usage: png2asset    <file>.png [options]\n");
        printf("-o <filename>       ouput file (if not used then default is <png file>.c)\n");
        printf("-c <filename>       deprecated, same as -o\n");
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
        printf("-sprite_no_optimize keep empty sprite tiles, do not remote duplicate tiles\n");
        printf("-b <bank>           bank (default: fixed bank)\n");
        printf("-keep_palette_order use png palette\n");
        printf("-repair_indexed_pal try to repair indexed tile palettes (implies \"-keep_palette_order\")\n");
        printf("-noflip             disable tile flip\n");
        printf("-map                Export as map (tileset + bg) instead of default metasprite output\n");
        printf("-use_map_attributes Use CGB BG Map attributes\n");
        printf("-use_nes_attributes Use NES BG Map attributes\n");
        printf("-use_nes_colors     Convert RGB color values to NES PPU colors\n");
        printf("-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)\n");
        printf("-bpp                bits per pixel: 1, 2, 4 (default: 2)\n");
        printf("-max_palettes       max number of palettes allowed (default: 8)\n");
        printf("                    (note: max colors = max_palettes x num colors per palette)\n");
        printf("-pack_mode          gb, nes, sgb, sms, 1bpp (default: gb)\n");
        printf("-tile_origin        tile index offset for maps (default: 0)\n");

        printf("-tiles_only         export tile data only\n");
        printf("-maps_only          export map tilemap only\n");
        printf("-metasprites_only   export metasprite descriptors only\n");
        printf("-source_tileset     use source tileset (image with common tiles)\n");
        printf("-entity_tileset     (maps only) mark matching tiles counting from 255 down, entity patterns not exported\n");
        printf("-keep_duplicate_tiles   do not remove duplicate tiles (default: not enabled)\n");
        printf("-no_palettes        do not export palette data\n");

        printf("-bin                export to binary format\n");
        printf("-transposed         export transposed (column-by-column instead of row-by-row)\n");

        printf("-rel_paths          paths to tilesets are relative to the input file path\n");
        return 0;
    }

    //default params
    args->input_filename = argv[1];
    args->output_filename = argv[1];
    args->output_filename = args->output_filename.substr(0, args->output_filename.size() - 4) + ".c";

    //Parse argv
    for(int i = 2; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-sw"))
        {
            args->spriteSize.width = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-sh"))
        {
            args->spriteSize.height = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-sp"))
        {
            args->props_default = strtol(argv[++i], NULL, 16);
        }
        else if(!strcmp(argv[i], "-px"))
        {
            args->pivot.x = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-py"))
        {
            args->pivot.y = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-pw"))
        {
            args->pivot.width = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-ph"))
        {
            args->pivot.height = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-spr8x8"))
        {
            args->sprite_mode = SPR_8x8;
        }
        else if(!strcmp(argv[i], "-spr8x16"))
        {
            args->sprite_mode = SPR_8x16;
        }
        else if(!strcmp(argv[i], "-spr16x16msx"))
        {
            args->sprite_mode = SPR_16x16_MSX;
        }
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "-o"))
        {
            if ((i + 1) >= argc) {
                printf("Error: -c or -o requires a filename, none specified\n");
                return 1;
            } else if (argv[i+1][0] == '-') {
                printf("Error: next argument after -o looks like an option instead of a filename (\"%s\")\n", argv[i + 1]);
                return 1;
            }

            args->output_filename = argv[++i];
        }
        else if(!strcmp(argv[i], "-b"))
        {
            args->bank = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-keep_palette_order"))
        {
            args->keep_palette_order = true;
        }
        else if(!strcmp(argv[i], "-repair_indexed_pal"))
        {
            args->repair_indexed_pal = true;
            args->keep_palette_order = true; // -repair_indexed_pal requires -keep_palette_order, so force it on
        }
        else if(!strcmp(argv[i], "-noflip"))
        {
            args->flip_tiles = false;
        }
        else if(!strcmp(argv[i], "-map"))
        {
            args->export_as_map = true;
        }
        else if(!strcmp(argv[i], "-use_map_attributes"))
        {
            args->use_map_attributes = true;
        }
        else if(!strcmp(argv[i], "-use_nes_attributes"))
        {
            args->use_map_attributes = true;
            args->use_2x2_map_attributes = true;
            args->pack_map_attributes = true;
        }
        else if(!strcmp(argv[i], "-use_nes_colors"))
        {
            args->convert_rgb_to_nes = true;
        }
        else if(!strcmp(argv[i], "-use_structs"))
        {
            args->use_structs = true;
        }
        else if(!strcmp(argv[i], "-bpp"))
        {
            args->bpp = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-max_palettes"))
        {
            args->max_palettes = atoi(argv[++i]);
            if(args->max_palettes == 0)
            {
                printf("-max_palettes must be larger than zero\n");
                return 1;
            }
        }
        else if(!strcmp(argv[i], "-pack_mode"))
        {
            std::string pack_mode_str = argv[++i];
            if(pack_mode_str == "gb")  args->pack_mode = Tile::GB;
            else if(pack_mode_str == "nes") args->pack_mode = Tile::NES;
            else if(pack_mode_str == "sgb") {
                args->pack_mode = Tile::SGB;
                 // SGB attributes are packed in map data, so 2 bytes per map tile
                args->map_entry_size_bytes = 2;
            }
            else if(pack_mode_str == "sms") {
                args->pack_mode = Tile::SMS;
                 // SMS attributes are packed in map data, so 2 bytes per map tile
                args->map_entry_size_bytes = 2;
            }
            else if(pack_mode_str == "1bpp") args->pack_mode = Tile::BPP1;
            else
            {
                printf("-pack_mode must be one of gb, nes, sgb, sms, 1bpp\n");
                return 1;
            }
        }
        else if(!strcmp(argv[i], "-tile_origin"))
        {
            args->tile_origin = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-maps_only") || !strcmp(argv[i], "-metasprites_only"))
        {
            args->includeTileData = false;
        }
        else if(!strcmp(argv[i], "-tiles_only"))
        {
            args->includedMapOrMetaspriteData = false;
        }
        else if(!strcmp(argv[i], "-keep_duplicate_tiles"))
        {
            args->keep_duplicate_tiles = true;
        }
        else if(!strcmp(argv[i], "-sprite_no_optimize"))
        {
            args->keep_duplicate_tiles = true;
            args->keep_empty_sprite_tiles = true;
        }
        else if(!strcmp(argv[i], "-no_palettes"))
        {
            args->include_palettes = false;
        }
        else if(!strcmp(argv[i], "-source_tileset"))
        {
            args->includeTileData = false;
            args->source_tilesets.push_back(argv[++i]);
        }
        else if(!strcmp(argv[i], "-entity_tileset"))
        {
            args->entity_tileset_filename = argv[++i];
            args->has_entity_tileset = true;
        }
        else if(!strcmp(argv[i], "-bin"))
        {
            args->output_binary = true;
        }
        else if(!strcmp(argv[i], "-transposed"))
        {
            args->output_transposed = true;
        }
        else if(!strcmp(argv[i], "-rel_paths")) {
            args->relative_paths = true;
        }
        else {
            printf("Warning: Argument \"%s\" not recognized\n", argv[i]);
        }
    }

    int slash_pos = (int)args->output_filename.find_last_of('/');
    if(slash_pos == -1)
        slash_pos = (int)args->output_filename.find_last_of('\\');
    int dot_pos = (int)args->output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

    args->output_filename_h = args->output_filename.substr(0, dot_pos) + ".h";
    args->output_filename_bin = args->output_filename.substr(0, dot_pos) + "_map.bin";
    args->output_filename_attributes_bin = args->output_filename.substr(0, dot_pos) + "_map_attributes.bin";
    args->output_filename_tiles_bin = args->output_filename.substr(0, dot_pos) + "_tiles.bin";
    args->data_name = args->output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
    replace(args->data_name.begin(), args->data_name.end(), '-', '_');

    return 0;
}

