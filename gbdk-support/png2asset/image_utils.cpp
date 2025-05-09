
// image_utils.cpp

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include "lodepng.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"

static void image_bitunpack(const vector<uint8_t> & src_image_data, vector<uint8_t> & unpacked_image_data, uint8_t bitdepth);

static int pixel_get_palette_num(const PNGImage& image, int x, int y);
static bool tile_palette_is_ok(const PNGImage& image, int x, int y, int tile_w, int tile_h);
static int pixel_find_color_in_palette(const PNGImage& image, int x, int y, int pal_num, uint8_t * match_color_idx);
static bool tile_palette_try_repair(PNGImage& image, int x, int y, int tile_w, int tile_h, int pal);
static bool tile_palette_repair(PNGImage& image, int x, int y, int tile_w, int tile_h);


// Un-bitpacks a source image into an 8-bit-per-pixel destination buffer
// Acceptable range is 1-8 bits per pixel
static void image_bitunpack(const vector<uint8_t> & src_image_data, vector<uint8_t> & unpacked_image_data, uint8_t bitdepth) {

    int pixels_per_byte = (8 / bitdepth);

    // Loop through all pixels in source image buffer
    for (const uint8_t & it_src: src_image_data) {

        // Unpack grouped pixel data
        uint8_t packed_bits = it_src;
        for (int b = 0; b < pixels_per_byte; b++) {
            unpacked_image_data.push_back(packed_bits >> (8 - bitdepth)); // Save extracted pixel bits
            packed_bits <<= bitdepth;                                     // Shift out the extracted bits
        }
    }
}


// Converts indexed image with bit depths 1- 8 bpp to indexed 8 bits per pixel
// Replaces incoming image buffer with unpacked image buffer if successful
bool image_indexed_ensure_8bpp(vector<uint8_t> & src_image_data, int bitdepth, int colortype) {

    vector<uint8_t> unpacked_image_data;

    if (colortype != LCT_PALETTE) {
        printf("png2asset: Error: keep_palette_order only works with indexed png images (pngtype: %d, bits:%d)\n",
               colortype, bitdepth);
        return false;
    }
    else if ((bitdepth < 1) || (bitdepth > 8)) {
        printf("png2asset: Error: Indexed mode PNG requires between 1 - 8 bits per pixel (pngtype: %d, bits:%d)\n",
                colortype, bitdepth);
        return false;
    }

    image_bitunpack(src_image_data, unpacked_image_data, bitdepth);

    // Replace source image pixel data with unpacked pixels
    src_image_data.clear();
    for (const uint8_t & it_unpacked: unpacked_image_data)
        src_image_data.push_back(it_unpacked);

    return true;
}




// Returns the sub-palette number for a pixel in a indexed 8bpp image
static int pixel_get_palette_num(const PNGImage& image, int x, int y) {

    return image.data[(y * image.w) + x] / (unsigned int)image.colors_per_pal;
}


// Verifies that a tile uses only colors within it's sub-palette in a indexed 8bpp image
static bool tile_palette_is_ok(const PNGImage& image, int x, int y, int tile_w, int tile_h) {

    // Get palette from first pixel in tile, use that as reference
    int prev_palette_num = pixel_get_palette_num(image, x, y);

    for (int tile_y = 0; tile_y < tile_h; tile_y++) {
        for (int tile_x = 0; tile_x < tile_w; tile_x++) {

            // If any pixels have a different palette than it's a failure
            if (prev_palette_num != pixel_get_palette_num(image, x + tile_x, y + tile_y)) {
                return false; // Return failure
            }
        }
    }
    return true; // Return success
}


// Finds a matching sub-palette RGB color for a pixel in a indexed 8bpp image
static int pixel_find_color_in_palette(const PNGImage& image, int x, int y, int pal_num, uint8_t * match_color_idx) {

    // Get pixel palette index num, then get RGB value for it
    uint8_t pixel_pal_idx = image.data[(y * image.w) + x];
    auto p_pixel_color = &image.palette[pixel_pal_idx * RGBA32_SZ];

    // Loop through available palette colors to see if an exact match exists
    auto p_pal_color = &image.palette[pal_num * (image.colors_per_pal * RGBA32_SZ)];
    for (int c = 0; c < (int)image.colors_per_pal; c++, p_pal_color += RGBA32_SZ) {

        // Check palette entry RGB match against pixel RGB
        if ((p_pal_color[0] == p_pixel_color[0]) &&
            (p_pal_color[1] == p_pixel_color[1]) &&
            (p_pal_color[2] == p_pixel_color[2])) {

            *match_color_idx = (pal_num * (int)image.colors_per_pal) + c;
            return true; // Return success
        }
    }

    return false; // Return failure
}


// Tries to remap a tile to a new palette. Fails if any color in tile isn't in palette
// Note: For any color that does match, tile color indexed gets remapped even if the rest
//       of the match fails. That's ok because the new color is visually identical to previous.
static bool tile_palette_try_repair(PNGImage& image, int x, int y, int tile_w, int tile_h, int pal_num) {

    uint8_t new_color;

    // For each pixel in the tile, try to find an exact color match in the palette
    for (int tile_y = 0; tile_y < tile_h; tile_y++) {
        for (int tile_x = 0; tile_x < tile_w; tile_x++) {

            // Update pixel if exact match was found
            // Otherwise quit as soon as possible to reduce processing
            if (pixel_find_color_in_palette(image, x + tile_x, y + tile_y, pal_num, &new_color))
                image.data[((y + tile_y) * image.w) + x + tile_x] = new_color;
            else
                return false; // Failed, tile color not present in palette
        }
    }

    // printf("  + png2asset: Info: repaired tile at %d x %d to palette make%d\n", x, y, pal_num);
    return true; // Success, found a perfect match
}


// Checks all palettes to see if the tile's colors can be remapped to them.
static bool tile_palette_repair(PNGImage& image, int x, int y, int tile_w, int tile_h) {

    // Loop through all sub-palettes
    for (int pal_num = 0; pal_num < (int)image.total_color_count / (int)image.colors_per_pal; pal_num++)
        if (tile_palette_try_repair(image, x, y, tile_w, tile_h, pal_num))
            return true; // Success

    printf("png2asset: Error: tile (%dx%d) at %d,%d - %d,%d uses unique colors from different palettes, "
           "cannot repair palette\n", x / tile_w, y / tile_h, x, y, x + tile_w - 1, y + tile_h - 1);

    // If no exact palette match was found then signal failure
    return false; // Failure
}


// Tries to repair indexed color sub-palette usage per attribute tile region
// Must find a palette that contains all colors in tile, fails otherwise
//
// Image passed in must be: indexed 8bpp
bool image_indexed_repair_tile_palettes(PNGImage & image, bool use_2x2_map_attributes) {

    // TODO: Optional, but need to pass in vars
    // if ((colortype != LCT_PALETTE) || (bitdepth != 8)) {
    //     printf("png2asset: Error: repair_tile_palettes only works with indexed png images in 8 bits per pixel mode");
    //     return false;
    // }

    int tile_w = image.tile_w;
    int tile_h = image.tile_h;

    if (use_2x2_map_attributes) {
        tile_w *= 2;
        tile_h *= 2;
    }

    for (int y = 0; y < (int)image.h; y += tile_h) {
        for (int x = 0; x < (int)image.w; x += tile_w) {
            // If tile has no palette errors leave it as is
            // but if there are errors then try to repair it
            if (!tile_palette_is_ok(image, x, y, tile_w, tile_h)) {
                if (!tile_palette_repair(image, x, y, tile_w, tile_h)) {
                    return false; // Return Failure
                }
            }
        }
    }

    return true; // Return success
}

