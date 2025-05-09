#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>
#include <cstdlib>

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
    args->enable_layered_sprites = false;

    // args->errorCode;
    args->bank = BANK_NUM_UNSET;
    args->area_specified = false;
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
        printf("-spr8x8             for 8x8  hardware sprites (use SPRITES_8x8)\n");
        printf("-spr8x16            for 8x16 hardware sprites (use SPRITES_8x16) (the default)\n");
        printf("-spr16x16msx        MSX only: for 16x16 hardware sprites (use SPRITES_16x16)\n");
        printf("-sprite_no_optimize keep empty sprite tiles, do not remove duplicate tiles\n");
        printf("-b <banknum>        Bank number (default: fixed bank)\n");
        printf("-area <area name>   Area name. Alters \"pragma bank ...\" output to constseg style. (Ex: -area LIT)\n");
        printf("-keep_palette_order use png palette\n");
        printf("-repair_indexed_pal try to repair indexed tile palettes (implies \"-keep_palette_order\")\n");
        printf("-noflip             disable tile flip\n");
        printf("-map                Export as map (tileset + bg) instead of default metasprite output\n");
        printf("-use_map_attributes Use CGB BG Map attributes\n");
        printf("-use_nes_attributes Use NES BG Map attributes\n");
        printf("-use_nes_colors     Convert RGB color values to NES PPU colors\n");
        printf("-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)\n");
        printf("-bpp                bits per pixel: 1, 2, 4 (default: 2. using 1 auto-enables \"-pack_mode 1bpp\")\n");
        printf("-max_palettes       max number of palettes allowed (default: 8)\n");
        printf("                    (note: max colors = max_palettes x num colors per palette)\n");
        printf("-pack_mode          gb, nes, sgb, sms, 1bpp (default: gb. using 1bpp auto-enables \"-bpp 1\")\n");
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
        printf("-layered-sprites    allows for automatically handling multi-palette tiles.\n");
        return EXIT_SUCCESS;
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
                return EXIT_FAILURE;
            } else if (argv[i+1][0] == '-') {
                printf("Error: next argument after -o looks like an option instead of a filename (\"%s\")\n", argv[i + 1]);
                return EXIT_FAILURE;
            }

            args->output_filename = argv[++i];
        }
        else if(!strcmp(argv[i], "-b"))
        {
            args->bank = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-area"))
        {
            args->area_name = argv[++i];
            args->area_specified = true;
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
            // If set to "-bpp 1" auto-enable "-pack_mode 1bpp"
            if (atoi(argv[i]) == 1) args->pack_mode = Tile::BPP1;
        }
        else if(!strcmp(argv[i], "-max_palettes"))
        {
            args->max_palettes = atoi(argv[++i]);
            if(args->max_palettes == 0)
            {
                printf("-max_palettes must be larger than zero\n");
                return EXIT_FAILURE;
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
            else if(pack_mode_str == "1bpp") {
                args->pack_mode = Tile::BPP1;
                // If set to "-pack_mode 1bpp" auto-enable "-bpp 1"
                args->bpp = 1;
            }
            else
            {
                printf("-pack_mode must be one of gb, nes, sgb, sms, 1bpp\n");
                return EXIT_FAILURE;
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
            // Warning / TODO:FIXME
            // includeTileData gets overridden back to TRUE if any tiles not present in the source tileset are found.
            // This makes understanding it's full export behavior much harder. Would be better to remove it and try to simplify logic.
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
        else if(!strcmp(argv[i], "-layered-sprites")) {
            args->enable_layered_sprites = true;
        } else {
            printf("Warning: Argument \"%s\" not recognized\n", argv[i]);
        }
    }

    if(args->enable_layered_sprites) {

        /*
         * It's incredibly difficult to efficiently determine how to automatically generate palettes for layered sprites.
         * For this reason, the program will fail if the user isn't using a paletted PNGs.
         */
        if(!args->keep_palette_order) {
            printf("Error: The `-layered-sprites` argument also requires the `-keep_palette_order` argument.");
            return EXIT_FAILURE;
        }

        printf("Warning! Layered metasprites have been enabled. Each platform has limits on how many 'sprites' can drawn on a single scanline. Keep these numbes in mind when using layered metasprites.\n");
        printf("\tGB/AP/Duck:   10 Sprites per scanline\n");
        printf("\tSMS/GG:       8 Sprites per scanline\n");
        printf("\tNES/Famicom:  8 Sprites per scanline\n\n");
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

    if ((args->area_specified == true) && (args->bank == BANK_NUM_UNSET)) {
        printf("Error: \"-area\" specified but bank number is missing. A bank number is required, use \"-b\"\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

