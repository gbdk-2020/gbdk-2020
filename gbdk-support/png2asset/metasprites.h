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
#include "map_attributes.h"
#include "palettes.h"
#include "source_tileset.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "maps.h"
#include "metasprites.h"



typedef vector< MTTile > MetaSprite;

void GetAllMetasprites();
void GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y);