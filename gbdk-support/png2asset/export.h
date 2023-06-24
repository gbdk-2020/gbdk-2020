#pragma once



#include "process_arguments.h"
#include "png2asset.h"

using namespace std;

bool export_h_file( PNG2AssetData* assetData);
bool export_c_file( PNG2AssetData* assetData);
bool export_map_binary( PNG2AssetData* assetData);
