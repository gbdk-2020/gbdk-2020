#pragma once

#include <set>
#include "png_image.h"

using namespace std;

//Functor to compare entries in SetPal
struct CmpIntColor {
    bool operator() (unsigned int const& c1, unsigned int const& c2) const
    {
        unsigned char* c1_ptr = (unsigned char*)&c1;
        unsigned char* c2_ptr = (unsigned char*)&c2;

        //Compare alpha first, transparent color is considered smaller
        if(c1_ptr[0] != c2_ptr[0])
        {
            return c1_ptr[0] < c2_ptr[0];
        }
        else
        {
            // Do a compare with luminance in upper bits, and original rgb24 in lower bits. 
            // This prefers luminance, but considers RGB values for equal-luminance cases to
            // make sure the compare functor satisifed the strictly weak ordering requirement
            uint64_t lum_1 = (unsigned int)(c1_ptr[3] * 299 + c1_ptr[2] * 587 + c1_ptr[1] * 114);
            uint64_t lum_2 = (unsigned int)(c2_ptr[3] * 299 + c2_ptr[2] * 587 + c2_ptr[1] * 114);
            uint64_t rgb_1 = (c1_ptr[3] << 16) | (c1_ptr[2] << 8) | c1_ptr[1];
            uint64_t rgb_2 = (c2_ptr[3] << 16) | (c2_ptr[2] << 8) | c2_ptr[1];
            uint64_t all_1 = (lum_1 << 24) | rgb_1;
            uint64_t all_2 = (lum_2 << 24) | rgb_2;
            return all_1 > all_2;
        }
    }
};

//This set will keep colors in the palette ordered based on their grayscale values to ensure they look good on DMG
//This assumes the palette used in DMG will be 00 01 10 11
typedef set< unsigned int, CmpIntColor > SetPal;


SetPal GetPaletteColors(const PNGImage& image, int x, int y, int w, int h);