#pragma once

#include "process_arguments.h"
#include "png2asset.h"

using namespace std;

struct exportOptions_t {

    size_t color_count, color_start;
    size_t tiles_count, tiles_start;
    bool has_palette_data_to_export;
};

string extract_name(string const & name);
void calc_palette_and_tileset_export_size(PNG2AssetData* assetData, exportOptions_t* exportOptions);
