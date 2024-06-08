// image_utils.h

#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H

#include <vector>

bool image_indexed_ensure_8bpp(vector <unsigned char> &data, int bitdepth, int colortype);
bool image_indexed_repair_tile_palettes(PNGImage & image, bool use_2x2_map_attributes);

#endif