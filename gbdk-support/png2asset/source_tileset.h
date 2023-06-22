#pragma once

#include <vector>
#include "process_arguments.h"
#include "cmp_int_color.h"
using namespace std;
bool GetSourceTileset(bool repair_indexed_pal, bool keep_palette_order, unsigned int max_palettes, vector< SetPal >& palettes, PNG2AssetData* png2AssetData);

int HandleSourceTileset(PNG2AssetData* png2AssetData);