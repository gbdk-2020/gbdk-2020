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
#include "process_arguments.h"

using namespace std;

#include "png_image.h"
#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"
#include "png_image.h"

int decodePNG(vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
void loadFile(vector<unsigned char>& buffer, const std::string& filename);

int ReadImageData_KeepPaletteOrder(  PNG2AssetData* assetData, string input_filename) {

    //load and decode png
    vector<unsigned char> buffer;
    lodepng::load_file(buffer, input_filename);
    lodepng::State state;

    //Calling with keep_palette_order means
        //-The image should be png indexed (1-8 bits per pixel)
        //-For CGB: Each 4 colors define a gbc palette, the first color is the transparent one
        //-Each rectangle with dimension(tile_w, tile_h) in the image has colors from one of those palettes only
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;

    // * Do *NOT* turn color_convert ON here.
    // When source PNG is indexed with bit depth was less than 8 bits per pixel then
    // color_convert may mangle the packed indexed values. Instead manually unpack them.
    //
    // This is necessary since some PNG encoders will use the minimum possible number of bits.
    //   For example 2 colors in the palette -> 1bpp -> 8 pixels per byte in the decoded image.
    //     Also see below about requirement to use palette from source image
    state.decoder.color_convert = false;

    // Clearing is needed to ensure loading the png works
    // (in cases where a previous png was loaded for source or entity tilesets)
    assetData->image.data.clear();

    unsigned error = lodepng::decode(assetData->image.data, assetData->image.w, assetData->image.h, state, buffer);
    // Unpack the image if needed. Also checks and errors on incompatible palette type if needed
    if(!image_indexed_ensure_8bpp(assetData->image.data, (int)state.info_png.color.bitdepth, (int)state.info_png.color.colortype))
        return 1;
    else if(error) {
        printf("decoder error %s\n", lodepng_error_text(error));
        return 1;
    }

    // Use source image palette since lodepng conversion to indexed (LCT_PALETTE) won't create a palette
    // So: state->info_png.color.palette/size instead of state->info_raw.palette/size
    unsigned int palette_count = PaletteCountApplyMaxLimit((unsigned int)assetData->args->max_palettes, (unsigned int)((state.info_png.color.palettesize + assetData->image.colors_per_pal - 1) / assetData->image.colors_per_pal));
    assetData->image.total_color_count = palette_count * assetData->image.colors_per_pal;
    // Copy the palette data since the source buffer (state...) won't exist outside the scope of this function
    assetData->image.palette = new unsigned char[assetData->image.total_color_count * RGBA32_SZ];
    memcpy(assetData->image.palette, state.info_png.color.palette, assetData->image.total_color_count * RGBA32_SZ);

    // Save a copy of the palette data if it's the source palette (free first if already allocated)
    if (assetData->args->processing_mode == MODE_SOURCE_TILESET) {
        if (assetData->image.source_tileset_palette) free(assetData->image.source_tileset_palette);
        assetData->image.source_tileset_palette = new unsigned char[assetData->image.total_color_count * RGBA32_SZ];
        memcpy(assetData->image.source_tileset_palette, state.info_png.color.palette, assetData->image.total_color_count * RGBA32_SZ);
    }


    if(assetData->args->repair_indexed_pal)
        if(!image_indexed_repair_tile_palettes(assetData->image, assetData->args->use_2x2_map_attributes))
            return 1;

    // TODO: Enable dimension check
    // // Validate image dimensions
    // if (((image.w % tile_w) != 0) || ((image.h % tile_h) != 0))
    // {
    //     printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image.w, image.h, tile_w, tile_h);
    //     return 1;
    // }

    // Only check this for the main image, not for source tilesets
    if ((assetData->args->processing_mode == MODE_MAIN_IMAGE) && (assetData->args->has_source_tilesets)) {

        // Make sure these two values match when keeping palette order
        if(assetData->image.total_color_count != assetData->args->source_total_color_count) {

            printf("error: The number of color palettes for your source tileset (%d) and target image (%d) do not match.",
                   (unsigned int)assetData->args->source_total_color_count, (unsigned int)assetData->image.total_color_count);
            return 1;
        }

        size_t size = max(assetData->image.total_color_count, assetData->args->source_total_color_count);
        // size_t size = max(assetData->image.total_color_count, assetData->source_tileset_image.total_color_count);

        // Make sure these two values match when keeping palette order
        if (memcmp(assetData->image.palette, assetData->image.source_tileset_palette, size) != 0) {

            printf("error: The palettes for your source tileset and target image do not match.");
            return 1;
        }
    }
    return 0;
}

int ReadImageData_Default(PNG2AssetData* assetData, string  input_filename) {

    //load and decode png
    vector<unsigned char> buffer;
    lodepng::load_file(buffer, input_filename);
    lodepng::State state;

    PNGImage image32;
    image32.colors_per_pal = assetData->image.colors_per_pal;
    image32.tile_w = assetData->image.tile_w;
    image32.tile_h = assetData->image.tile_h;

    unsigned error = lodepng::decode(image32.data, image32.w, image32.h, state, buffer); //decode as 32 bit
    if(error)
    {
        printf("decoder error %s\n", lodepng_error_text(error));
        return 1;
    }

    // Validate image dimensions
    if(((image32.w % image32.tile_w) != 0) || ((image32.h % image32.tile_h) != 0))
    {
        printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image32.w, image32.h, image32.tile_w, image32.tile_h);
        return 1;
    }

    int* palettes_per_tile = BuildPalettesAndAttributes(image32, assetData->palettes, assetData->args->use_2x2_map_attributes);

    // Create the indexed image
    // Clearing is needed to ensure loading the png works
    // (in cases where a previous png was loaded for source or entity tilesets)
    assetData->image.data.clear();
    assetData->image.w = image32.w;
    assetData->image.h = image32.h;

    unsigned int palette_count = PaletteCountApplyMaxLimit((unsigned int)assetData->args->max_palettes, (unsigned int)assetData->palettes.size());

    assetData->image.total_color_count = palette_count * assetData->image.colors_per_pal;
    assetData->image.palette = new unsigned char[palette_count * assetData->image.colors_per_pal * RGBA32_SZ]; // total color count * 4 bytes each
    // Pre-fill palette to all black. Prevents garbage in palette color slots that are unused (ex: only 3 colors when colors-per-pal is 4)
    memset(assetData->image.palette, 0, palette_count * assetData->image.colors_per_pal * RGBA32_SZ);

    // If using a source tileset and have more palettes than it defines
    // (only check for main image data, not source tilesets)
    if ((assetData->args->processing_mode == MODE_MAIN_IMAGE) && (assetData->args->has_source_tilesets)) {
        if (assetData->image.total_color_count > assetData->args->source_total_color_count) {
            printf("Found %d extra palette(s) for target tilemap.\n", (unsigned int)((assetData->image.total_color_count - assetData->args->source_total_color_count) / assetData->image.colors_per_pal));
        }
    }
    for(size_t p = 0; p < palette_count; ++p)
    {
        int* color_ptr = (int*)&assetData->image.palette[p * assetData->image.colors_per_pal * RGBA32_SZ];

        // When palettes[p].size() != image.colors_per_pal the unused colors are left as black (see memset above)
        for(SetPal::iterator it = assetData->palettes[p].begin(); it != assetData->palettes[p].end(); ++it, color_ptr++)
        {
            unsigned char* c = (unsigned char*)&(*it);
            *color_ptr = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
        }
    }

    for(size_t y = 0; y < image32.h; ++y)
    {
        for(size_t x = 0; x < image32.w; ++x)
        {
            unsigned char* c32ptr = &image32.data[(image32.w * y + x) * RGBA32_SZ];
            int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
            unsigned char palette = palettes_per_tile[(y / image32.tile_h) * (image32.w / image32.tile_w) + (x / image32.tile_w)];
            unsigned char index = (unsigned char)std::distance(assetData->palettes[palette].begin(), assetData->palettes[palette].find(color32));
            assetData->image.data.push_back((palette << assetData->args->bpp) + index);
        }
    }

    //Test: output png to see how it looks
    //Export(image, "temp.png");
    return 0;
}

int ReadImageData( PNG2AssetData* assetData, string  input_filename) {

    assetData->image.colors_per_pal = static_cast<size_t>(1) << assetData->args->bpp;
    // assetData->source_tileset_image.colors_per_pal = static_cast<size_t>(1) << assetData->args->bpp;

    if(assetData->args->export_as_map)
    {
        assetData->image.tile_w = 8; //Force tiles_w to 8 on maps
        assetData->image.tile_h = 8; //Force tiles_h to 8 on maps
        // assetData->source_tileset_image.tile_w = 8; //Force tiles_w to 8 on maps
        // assetData->source_tileset_image.tile_h = 8; //Force tiles_h to 8 on maps
        assetData->args->sprite_mode = SPR_NONE;
    }


    int errorCode = 0;

    // Will the specified PNG have a palette provided with it?
    if(assetData->args->keep_palette_order) {
        
        // Save the error code
        errorCode= ReadImageData_KeepPaletteOrder(assetData, input_filename);
    }
    else
    {

        // Save the error code
        errorCode= ReadImageData_Default(assetData, input_filename);
    }

    if(errorCode != 0)return errorCode;


    // Only set this data when processing the main image
    if (assetData->args->processing_mode == MODE_MAIN_IMAGE) {
        if(assetData->args->spriteSize.width == 0)    assetData->args->spriteSize.width = (int)assetData->image.w;
        if(assetData->args->spriteSize.height == 0)   assetData->args->spriteSize.height = (int)assetData->image.h;
        if(assetData->args->pivot.x == 0xFFFFFF)      assetData->args->pivot.x = (unsigned int)assetData->args->spriteSize.width / 2;
        if(assetData->args->pivot.y == 0xFFFFFF)      assetData->args->pivot.y = (unsigned int)assetData->args->spriteSize.height / 2;
        if(assetData->args->pivot.width == 0xFFFFFF)  assetData->args->pivot.width = assetData->args->spriteSize.width;
        if(assetData->args->pivot.height == 0xFFFFFF) assetData->args->pivot.height = assetData->args->spriteSize.height;
    }
    return 0;
}

