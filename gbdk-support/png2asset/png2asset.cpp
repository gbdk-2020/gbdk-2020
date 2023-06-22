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


int main(int argc, char* argv[])
{
	PNG2AssetData png2AssetData(argc, argv);

	int proccesArgValue = png2AssetData.errorCode;

	// Return the error code if the function returns non-zero
	if(proccesArgValue != 0) {
		return proccesArgValue;
	}

	return png2AssetData.Execute();
}
