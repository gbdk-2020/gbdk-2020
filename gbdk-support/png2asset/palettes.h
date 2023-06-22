#pragma once
#include <vector>

#include "cmp_int_color.h"
#include "png2asset.h"

using namespace std;


unsigned int PaletteCountApplyMaxLimit(unsigned int max_palettes, unsigned int cur_palette_size);

int FindOrCreateSubPalette(const SetPal& pal, vector< SetPal >& palettes, size_t colors_per_pal);


int* BuildPalettesAndAttributes(const PNGImage& image32, vector< SetPal >& palettes, bool half_resolution);