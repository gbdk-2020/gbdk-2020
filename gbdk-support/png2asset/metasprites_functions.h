#pragma once
#include <vector>
#include "metasprites.h"
#include "process_arguments.h"


bool GetAllMetasprites(PNG2AssetData* assetData);
bool GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y, PNG2AssetData* assetData);