
// image_utils.cpp

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include "lodepng.h"

using namespace std;

#include "image_utils.h"

static void image_bitunpack(const vector<uint8_t> & src_image_data, vector<uint8_t> & unpacked_image_data, uint8_t bitdepth);


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
bool image_indexed_ensure_8bpp(vector<uint8_t> & src_image_data, int width, int height, int bitdepth, int colortype) {

    vector<uint8_t> unpacked_image_data;

    if (colortype != LCT_PALETTE) {
        printf("png2asset: Error: keep_palette_order only works with indexed png images");
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

