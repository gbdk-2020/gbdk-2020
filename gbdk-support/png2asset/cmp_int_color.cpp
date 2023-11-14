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

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "png_image.h"
#include "image_utils.h"

SetPal GetPaletteColors(const PNGImage& image, int x, int y, int w, int h)
{
    SetPal ret;
    for(int j = y; j < (y + h); ++j)
    {
        for(int i = x; i < (x + w); ++i)
        {
            const unsigned char* color = &image.data[(j * image.w + i) * RGBA32_SZ];
            int color_int = (color[0] << 24) | (color[1] << 16) | (color[2] << 8) | color[3];
            ret.insert(color_int);
        }
    }

    for(SetPal::iterator it = ret.begin(); it != ret.end(); ++it)
    {
        if(it != ret.begin() && ((0xFF & *it) != 0xFF)) //ret.begin() should be the only one transparent
            printf("Warning: found more than one transparent color in tile at x:%d, y:%d of size w:%d, h:%d\n", x, y, w, h);
    }

    return ret;
}