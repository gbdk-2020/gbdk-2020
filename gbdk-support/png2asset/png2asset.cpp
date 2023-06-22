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
#include "metasprites_functions.h"


int RunPNG2Asset(PNG2AssetData* png2AssetData){

	// This was moved from outside the upcoming else statement when not using keep_palette_order
	// So the 'GetSourceTileset' function can pre-populate it from the source tileset
	vector< SetPal > palettes;

	// if we are using a source tileset
	if(png2AssetData->use_source_tileset) {

		// Handle generation of the source tileset
		int handleSourceTilesetValue = HandleSourceTileset(palettes, png2AssetData);

		// Return the error code if the function returns non-zero
		if(handleSourceTilesetValue != 0) {
			return handleSourceTilesetValue;
		}
	}

	int readImageDataValue = ReadImageData(palettes, png2AssetData);

	// Return the error code if the function returns non-zero
	if(readImageDataValue != 0) {
		return readImageDataValue;
	}

	// Get the data depending on the type
	if(png2AssetData->export_as_map)GetMap(palettes, png2AssetData);
	else GetAllMetasprites( png2AssetData);

	return HandleExport(png2AssetData);
}

int main(int argc, char* argv[])
{
	PNG2AssetData png2AssetData;

	int proccesArgValue = ProcessArguments(argc, argv, &png2AssetData);

	// Return the error code if the function returns non-zero
	if(proccesArgValue != 0) {
		return proccesArgValue;
	}

	return RunPNG2Asset(&png2AssetData);
}
