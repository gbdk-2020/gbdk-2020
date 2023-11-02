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

int main(int argc, char* argv[])
{
	int errorCode = 0;

	// Read all arguments
	PNG2AssetArguments arguments;

	// Make sure we had no errors
	if((errorCode = processPNG2AssetArguments(argc, argv, &arguments)) != 0) {
		return errorCode;
	}

	PNG2AssetData png2AssetInstance;

	// If we have a source tileset
	if(arguments.source_tilesets.size() > 0) {

		vector<string>::iterator sourceTilesetsIterator = arguments.source_tilesets.begin();

		// Iterate through each source tileset and execute
        while (sourceTilesetsIterator < arguments.source_tilesets.end()) {

			// Run with our source tileset filename
			errorCode = png2AssetInstance.Execute(&arguments, *sourceTilesetsIterator);

			// Return the error code if the function returns non-zero
			if(errorCode != 0) {
				return errorCode;
			}

            sourceTilesetsIterator++;
		}

		// Save these values for later usage on the main execution
		arguments.source_tileset_size = (unsigned int)png2AssetInstance.tiles.size();
		arguments.source_total_color_count = png2AssetInstance.image.total_color_count;
	}

	// Run the primary input file
	// Return the error code if the function returns non-zero
	if((errorCode = png2AssetInstance.Execute(&arguments, arguments.input_filename)) != 0) {
		return errorCode;
	}

	return png2AssetInstance.Export();
}