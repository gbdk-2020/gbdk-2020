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

int PNG2AssetData::Execute(PNG2AssetArguments* arguments, string  input_filename){

    this->args = arguments;

    if(arguments->sprite_mode == SPR_8x8)
    {
        this->image.tile_w = 8;
        this->image.tile_h = 8;
    }
    else if(arguments->sprite_mode == SPR_8x16)
    {
        this->image.tile_w = 8;
        this->image.tile_h = 16;
    }
    else if(arguments->sprite_mode == SPR_16x16_MSX)
    {
        this->image.tile_w = 16;
        this->image.tile_h = 16;
    }

    int readImageDataValue = ReadImageData(this, input_filename);

    // Return the error code if the function returns non-zero
    if(readImageDataValue != 0) {
        return readImageDataValue;
    }

    // Extract the data depending on what type it is
    if (this->args->processing_mode == MODE_ENTITY_TILESET) {
        // Entity tileset extracts into a separate tileset
        // For entity tilesets ALWAYS keep all of it's tiles, never deduplicate them
        ExtractTileset(this, this->entity_tiles, true);
    } else if (this->args->processing_mode == MODE_SOURCE_TILESET) {
        // Source tileset extracts into the main shared tileset
        ExtractTileset(this, this->tiles, this->args->keep_duplicate_tiles);
    } else if (this->args->export_as_map)
        GetMap(this);
    else
        GetAllMetasprites(this);

    return 0;
}

int PNG2AssetData::Export() {

    // Header file export
    if(export_h_file(this) == false) return 1; // Exit with Fail

    if((this->args->export_as_map) && (this->args->output_binary)) {
        // Handle special case of binary map export
        export_map_binary(this);
    }
    else {
        // Normal source file export
        if(export_c_file(this) == false) return 1; // Exit with Fail
    }

    return 0;
}


bool FindTile(const Tile& t, size_t& idx, unsigned char& props, vector< Tile > & tileset, PNG2AssetData* assetData)
{
    vector< Tile >::iterator it;
    it = find(tileset.begin(), tileset.end(), t);
    if(it != tileset.end())
    {
        idx = (size_t)(it - tileset.begin());
        props = assetData->args->props_default;
        return true;
    }
    if(assetData->args->flip_tiles)
    {
        Tile tile = FlipV(t);
        it = find(tileset.begin(), tileset.end(), tile);
        if(it != tileset.end())
        {
            idx = (size_t)(it - tileset.begin());
            props = assetData->args->props_default | (1 << 5);
            return true;
        }

        tile = FlipH(tile);
        it = find(tileset.begin(), tileset.end(), tile);
        if(it != tileset.end())
        {
            idx = (size_t)(it - tileset.begin());
            props = assetData->args->props_default | (1 << 5) | (1 << 6);
            return true;
        }

        tile = FlipV(tile);
        it = find(tileset.begin(), tileset.end(), tile);
        if(it != tileset.end())
        {
            idx = (size_t)(it - tileset.begin());
            props = assetData->args->props_default | (1 << 6);
            return true;
        }
    }

    return false;
}