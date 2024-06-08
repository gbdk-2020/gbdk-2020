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


string extract_name(string const & name)
{
    int p = name.find_last_of("/\\");
    string res = (p > 0) ? name.substr(p + 1) : name;
    p = name.find_first_of(".");
    return (p > 0) ? res.substr(0, p) : res;
}


// Calculate some shared export settings (.c, .h, binary)
// This gets called at the start of each export output function
void calc_palette_and_tileset_export_size(PNG2AssetData* assetData, exportOptions_t* exportOptions) {

    bool use_structs_with_source_tileset = (assetData->args->has_source_tilesets == true) && (assetData->args->use_structs == true);

    // (source_tileset + use_structs) is a special combination for ZGB
    if (use_structs_with_source_tileset) {
        // Export all colors, including those from source tileset
        exportOptions->color_start = 0;
        exportOptions->color_count = (unsigned int)assetData->image.total_color_count;
    } else {
        // Otherwise default palette export behavior is to skip past/offset
        // palettes that were present in the source tileset
        exportOptions->color_start = assetData->args->source_total_color_count;
        exportOptions->color_count = assetData->image.total_color_count - assetData->args->source_total_color_count;
    }

    // Tile export behavior is to always skip past/offset
    // tiles that were present in the source tileset
    exportOptions->tiles_start = assetData->args->source_tileset_size;
    exportOptions->tiles_count = assetData->tiles.size() - assetData->args->source_tileset_size;

    // When to export palette data:
    //   - Not using a source tileset                        : include all colors
    //   - Using a source tileset and there are extra colors : include only colors not present in source tileset
    //
    //   AND it's not turned off via include_palettes = false
    // exportOptions->has_palette_data_to_export = (assetData->args->include_palettes &&
    //                               ((assetData->args->has_source_tilesets == false) || (exportOptions->color_count > 0)) );

    exportOptions->has_palette_data_to_export = (assetData->args->include_palettes &
                                  ((assetData->args->has_source_tilesets == false) || (exportOptions->color_count > 0) || (use_structs_with_source_tileset == true)));
}