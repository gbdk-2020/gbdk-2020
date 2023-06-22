#pragma once

#include "png2asset.h"
#include "process_arguments.h"


Tile FlipH(const Tile& tile);
Tile FlipV(const Tile& tile);
bool FindTile(const Tile& t, size_t& idx, unsigned char& props, PNG2AssetData* png2AssetData);
bool FindTile(const Tile& t, size_t& idx, unsigned char& props, PNG2AssetData* png2AssetData);