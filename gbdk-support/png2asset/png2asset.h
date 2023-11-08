#pragma once


#include <vector>
#include <string>
#include "vector2.h"
#include "mttile.h"
#include "metasprites.h"
#include "cmp_int_color.h"
#include "tiles.h"
#include "png_image.h"
#include "process_arguments.h"
using namespace std;

class PNG2AssetData {

public:

	int Execute(PNG2AssetArguments* arguments, string  input_filename);
	int Export();

	int errorCode;
	int layeredSpritesCount = 0;

	PNG2AssetArguments* args;

	vector< SetPal > palettes;
	vector< Tile > tiles;
	vector<	MetaSprite > sprites;
	vector< unsigned char > map;
	vector< unsigned char > map_attributes;
	PNGImage source_tileset_image;
	PNGImage image;

};


bool FindTile(const Tile& t, size_t& idx, unsigned char& props, PNG2AssetData* assetData);