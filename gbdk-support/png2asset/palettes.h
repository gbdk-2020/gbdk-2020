#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "global.h"
#include "mttile.h"
#include "export.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"


unsigned int PaletteCountApplyMaxLimit(unsigned int max_palettes, unsigned int cur_palette_size);

int FindOrCreateSubPalette(const SetPal& pal, vector< SetPal >& palettes, size_t colors_per_pal);


int* BuildPalettesAndAttributes(const PNGImage& image32, vector< SetPal >& palettes, bool half_resolution);