// image_utils.h

#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H

#include <vector>

bool image_indexed_ensure_8bpp(vector <unsigned char> &data, int width, int height, int bitdepth, int colortype);

#endif