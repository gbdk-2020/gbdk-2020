// This prevents windows build errors regarding the 'fopen' function
// Example: 'fopen': This function or variable may be unsafe.Consider using fopen_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.
// More info: https://stackoverflow.com/questions/14386/fopen-deprecated-warning
// From bbbbbr: `Looks like some of the benefits are concurrency protection, but that probably isn't an issue for png2asset`
#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

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

#include "cmp_int_color.h"
#include "metasprites.h"

#include "png2asset.h"
#include "png_image.h"
#include "export.h"
#include "image_utils.h"
#include "process_arguments.h"
#include "rgb_to_nes_lut.h"

using namespace std;

static exportOptions_t exportOpt;

static void export_map_binary_tile_data(PNG2AssetData* assetData);
static void export_map_binary_map_data(PNG2AssetData* assetData);
static void export_map_binary_palette_data(PNG2AssetData* assetData);

bool export_map_binary(PNG2AssetData* assetData) {

    calc_palette_and_tileset_export_size(assetData, &exportOpt);

    if (assetData->args->includeTileData)
        export_map_binary_tile_data(assetData);

    if (assetData->args->includedMapOrMetaspriteData)
        export_map_binary_map_data(assetData);

    if (exportOpt.has_palette_data_to_export)
        export_map_binary_palette_data(assetData);

    return true; // success
}


static void export_map_binary_tile_data(PNG2AssetData* assetData) {

    std::ofstream tilesBinaryFile;
    tilesBinaryFile.open(assetData->args->output_filename_tiles_bin, std::ios_base::binary);

    for(vector< Tile >::iterator it = assetData->tiles.begin() + exportOpt.tiles_start; it != assetData->tiles.end(); ++it)
    {

        vector< unsigned char > packed_data = (*it).GetPackedData(assetData->args->pack_mode, assetData->image.tile_w, assetData->image.tile_h, assetData->args->bpp);
        for(vector< unsigned char >::iterator it2 = packed_data.begin(); it2 != packed_data.end(); ++it2)
        {

            const char chars[] = { (const char)(*it2) };
            tilesBinaryFile.write(chars, 1);
        }

    }
    // Finalize the files
    tilesBinaryFile.close();
}


static void export_map_binary_map_data(PNG2AssetData* assetData) {

    std::ofstream mapBinaryFile, mapAttributesBinaryfile;
    mapBinaryFile.open(assetData->args->output_filename_bin, std::ios_base::binary);

    // Open our file for writing attributes if specified
    if (assetData->args->use_map_attributes && assetData->map_attributes.size()) {
        mapAttributesBinaryfile.open(assetData->args->output_filename_attributes_bin, std::ios_base::binary);
    }

    int columns = assetData->image.w >> 3;
    int rows = assetData->image.h >> 3;

    // If we want the values to be column-by-column
    if(assetData->args->output_transposed) {

        int map_entry_size = assetData->args->map_entry_size_bytes;
        // Swap the column/row for loops
        for(int column = 0; column < columns; column++) {
            for(int row = 0; row < rows; ++row) {

                int tile = (column + row * columns) * map_entry_size;

                // Write map items column-by-column
                mapBinaryFile.write( (const char *) &(assetData->map[tile]), map_entry_size);

                if (assetData->args->use_map_attributes && assetData->map_attributes.size()) {
                   mapAttributesBinaryfile.write( (const char*) &(assetData->map_attributes[tile]), map_entry_size);
                }
            }
        }
    }
    else {
        // Write the arrays as-is, row-by-row
        mapBinaryFile.write((const char*)(&assetData->map[0]), assetData->map.size());

        if (assetData->args->use_map_attributes && assetData->map_attributes.size()) {
            mapAttributesBinaryfile.write((const char*)(&assetData->map_attributes[0]), assetData->map_attributes.size());
        }
    }
    // Finalize the files
    mapBinaryFile.close();
    if (assetData->args->use_map_attributes && assetData->map_attributes.size()) {
        mapAttributesBinaryfile.close();
    }
}

static void export_map_binary_palette_data(PNG2AssetData* assetData) {

    std::ofstream paletteBinaryFile;
    paletteBinaryFile.open(assetData->args->output_filename_palettes_bin, std::ios_base::binary);

    const size_t palette_start = exportOpt.color_start / assetData->image.colors_per_pal;
    const size_t total_palette_count = assetData->image.total_color_count / assetData->image.colors_per_pal;
    int cur_color = exportOpt.color_start;

    for (size_t i = palette_start; i < total_palette_count; ++i)
    {
        unsigned char* pal_ptr = &assetData->image.palette[i * (assetData->image.colors_per_pal * RGBA32_SZ)];
        for (int c = 0; c < (int)assetData->image.colors_per_pal; ++c, pal_ptr += RGBA32_SZ)
        {
            if (assetData->args->convert_rgb_to_nes) {
                size_t rgb222 = (((pal_ptr[RGBA8_B] >> 6) & 0b00000011) << 4) |
                                (((pal_ptr[RGBA8_G] >> 6) & 0b00000011) << 2) |
                                (((pal_ptr[RGBA8_R] >> 6) & 0b00000011) << 0);
                paletteBinaryFile.write((const char *)&rgb_to_nes[rgb222], sizeof(rgb_to_nes[0]));
            }
            else if (assetData->args->pack_mode == Tile::SMS) {
                uint8_t rgb222 = (((pal_ptr[RGBA8_B] >> 6) & 0b00000011) << 4) |
                                 (((pal_ptr[RGBA8_G] >> 6) & 0b00000011) << 2) |
                                 (((pal_ptr[RGBA8_R] >> 6) & 0b00000011) << 0);
                paletteBinaryFile.write((const char *)&rgb222, sizeof(rgb222));
            }
            else if (assetData->args->pack_mode == Tile::GG) {
                uint16_t rgb444 = (((pal_ptr[RGBA8_B] >> 4) & 0b00001111) << 8) |
                                  (((pal_ptr[RGBA8_G] >> 4) & 0b00001111) << 4) |
                                  (((pal_ptr[RGBA8_R] >> 4) & 0b00001111) << 0);
                paletteBinaryFile.write((const char *)&rgb444, sizeof(rgb444));
            }
            else {
                uint16_t rgb555 = (((pal_ptr[RGBA8_B] >> 3) & 0b00011111) << 10) |
                                  (((pal_ptr[RGBA8_G] >> 3) & 0b00011111) << 5)  |
                                  (((pal_ptr[RGBA8_R] >> 3) & 0b00011111) << 0);
                paletteBinaryFile.write((const char *)&rgb555, sizeof(rgb555));
            }

            cur_color++;
        }
    }
    // Finalize the files
    paletteBinaryFile.close();
}
