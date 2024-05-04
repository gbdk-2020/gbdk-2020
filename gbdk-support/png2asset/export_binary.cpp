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


bool export_map_binary(PNG2AssetData* assetData) {

    calc_palette_and_tileset_export_size(assetData, &exportOpt);

    if (assetData->args->includeTileData)
        export_map_binary_tile_data(assetData);

    if (assetData->args->includedMapOrMetaspriteData)
        export_map_binary_map_data(assetData);

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
