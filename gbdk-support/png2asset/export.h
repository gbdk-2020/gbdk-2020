#pragma once



#include "process_arguments.h"
#include "png2asset.h"

using namespace std;

void Export(const PNGImage& image, const char* path,   PNG2AssetData* png2AssetData);
bool export_h_file( PNG2AssetData* png2AssetData);
bool export_c_file( PNG2AssetData* png2AssetData);
bool export_map_binary( PNG2AssetData* png2AssetData);

int HandleExport( PNG2AssetData* png2AssetData);