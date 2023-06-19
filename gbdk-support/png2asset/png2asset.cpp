#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "global.h"
#include "mttile.h"
#include "export.h"
#include "map_attributes.h"
#include "palettes.h"
#include "source_tileset.h"

#include "cmp_int_color.h"
#include "process_arguments.h"
#include "image_data.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"


int main(int argc, char* argv[])
{
	int proccesArgValue = ProcessArguments(argc, argv);

	// Return the error code if the function returns non-zero
	if(proccesArgValue != 0) {
		return proccesArgValue;
	}

	// This was moved from outside the upcoming else statement when not using keep_palette_order
	// So the 'GetSourceTileset' function can pre-populate it from the source tileset
	vector< SetPal > palettes;

	// if we are using a source tileset
	if(use_source_tileset) {

		// Handle generation of the source tileset
		int handleSourceTilesetValue = HandleSourceTileset(palettes);

		// Return the error code if the function returns non-zero
		if(handleSourceTilesetValue != 0) {
			return handleSourceTilesetValue;
		}
	}

	int readImageDataValue = ReadImageData(argc, argv,palettes);

	// Return the error code if the function returns non-zero
	if(readImageDataValue != 0) {
		return readImageDataValue;
	}

	// Get the data depending on the type
	if(export_as_map)GetMap(palettes);
	else GetAllMetasprites();

	return HandleExport();
}
