#pragma once

#include <vector>
#include "cmp_int_color.h"
#include "process_arguments.h"
using namespace std;


void GetMap( PNG2AssetData* assetData);
void ExtractTileset(PNG2AssetData* assetData, vector< Tile > & tileset, bool keep_duplicate_tiles);