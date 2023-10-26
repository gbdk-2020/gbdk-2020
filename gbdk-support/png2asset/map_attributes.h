#pragma once
#include <vector>
#include "process_arguments.h"
#include "cmp_int_color.h"
using namespace std;
unsigned char GetMapAttribute(size_t x, size_t y);
void ReduceMapAttributes2x2(const vector< SetPal >& palettes);
void AlignMapAttributes(PNG2AssetData* assetData);
void PackMapAttributes(PNG2AssetData* assetData);
void HandleMapAttributes( PNG2AssetData* assetData);