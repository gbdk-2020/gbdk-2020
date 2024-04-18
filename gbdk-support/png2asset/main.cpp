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

#include "png2asset.h"
#include "image_utils.h"
#include "image_data.h"
#include "maps.h"
#include "metasprites.h"
#include "metasprites_functions.h"
#include "process_arguments.h"
#include "png_image.h"
#include "tiles.h"

using namespace std;

string extract_path(bool extract, string const & path, string const & name)
{
    int p = path.find_last_of("/\\");
    return ((extract) && (p > 0)) ? path.substr(0, p + 1) + name: name;
}

int main(int argc, char* argv[])
{
    int errorCode = 0;

    // Read all arguments
    PNG2AssetArguments arguments;

    // Make sure we had no errors
    if((errorCode = processPNG2AssetArguments(argc, argv, &arguments)) != 0) {
        return errorCode;
    }

    // The png2AssetInstance tile and palette data is retained after
    // processing source tilesets and so is shared with the main image
    PNG2AssetData png2AssetInstance;

    // If we have a source tileset
    if (arguments.source_tilesets.size() > 0) {

        vector<string>::iterator sourceTilesetFileNameIter = arguments.source_tilesets.begin();

        // Iterate through each source tileset and execute
        while (sourceTilesetFileNameIter < arguments.source_tilesets.end()) {
            // Run with current source tileset filename
            arguments.processing_mode = MODE_SOURCE_TILESET;
            errorCode = png2AssetInstance.Execute(&arguments, extract_path(arguments.relative_paths, arguments.input_filename, *sourceTilesetFileNameIter));

            // Return the error code if the function returns non-zero
            if(errorCode != 0) {
                return errorCode;
            }

            sourceTilesetFileNameIter++;
        }

        // Save these values for later usage on the main execution
        arguments.source_tileset_size      = (unsigned int)png2AssetInstance.tiles.size();
        arguments.source_total_color_count = png2AssetInstance.image.total_color_count;

        // Clearing map and attributes isn't needed here since adding them is blocked for source tileset data
        // (pre-refactor png2asset used map.clear() and map_attributes.clear() )

        arguments.has_source_tilesets = true;
        printf("Got %d tiles from the source tileset.\n", (unsigned int)arguments.source_tileset_size);
        printf("Got %d palettes from the source tileset.\n", (unsigned int)(arguments.source_total_color_count / png2AssetInstance.image.colors_per_pal));
    }

    // If there is an entity tileset
    if (!arguments.entity_tileset_filename.empty()) {
        // Run with entity tileset filename
        arguments.processing_mode = MODE_ENTITY_TILESET;
        errorCode = png2AssetInstance.Execute(&arguments, extract_path(arguments.relative_paths, arguments.input_filename, arguments.entity_tileset_filename));

        // Return the error code if the function returns non-zero
        if(errorCode != 0) {
            return errorCode;
        }

        arguments.has_entity_tileset = true;
        printf("Got %d tiles from the entity tileset.\n", (unsigned int)(unsigned int)png2AssetInstance.entity_tiles.size());
    }

    // Run the primary input file
    // Return the error code if the function returns non-zero
    arguments.processing_mode = MODE_MAIN_IMAGE;
    if((errorCode = png2AssetInstance.Execute(&arguments, arguments.input_filename)) != 0) {
        return errorCode;
    }

    return png2AssetInstance.Export();
}