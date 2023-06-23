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


using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "process_arguments.h"


Tile FlipH(const Tile& tile)
{
	Tile ret;
	for(int j = (int)tile.data.size() - 8; j >= 0; j -= 8)
	{
		for(int i = 0; i < 8; ++i)
		{
			ret.data.push_back(tile.data[j + i]);
		}
	}
	ret.pal = tile.pal;
	return ret;
}

Tile FlipV(const Tile& tile)
{
	Tile ret;
	for(int j = 0; j < (int)tile.data.size(); j += 8)
	{
		for(int i = 7; i >= 0; --i)
		{
			ret.data.push_back(tile.data[j + i]);
		}
	}
	ret.pal = tile.pal;
	return ret;
}

bool FindTile(const Tile& t, size_t& idx, unsigned char& props,PNG2AssetData* png2AssetData)
{
	vector< Tile >::iterator it;
	it = find(png2AssetData->tiles.begin(), png2AssetData->tiles.end(), t);
	if(it != png2AssetData->tiles.end())
	{
		idx = (size_t)(it - png2AssetData->tiles.begin());
		props = png2AssetData->arguments.props_default;
		return true;
	}

	if(png2AssetData->arguments.flip_tiles)
	{
		Tile tile = FlipV(t);
		it = find(png2AssetData->tiles.begin(), png2AssetData->tiles.end(), tile);
		if(it != png2AssetData->tiles.end())
		{
			idx = (size_t)(it - png2AssetData->tiles.begin());
			props = png2AssetData->arguments.props_default | (1 << 5);
			return true;
		}

		tile = FlipH(tile);
		it = find(png2AssetData->tiles.begin(), png2AssetData->tiles.end(), tile);
		if(it != png2AssetData->tiles.end())
		{
			idx = (size_t)(it - png2AssetData->tiles.begin());
			props = png2AssetData->arguments.props_default | (1 << 5) | (1 << 6);
			return true;
		}

		tile = FlipV(tile);
		it = find(png2AssetData->tiles.begin(), png2AssetData->tiles.end(), tile);
		if(it != png2AssetData->tiles.end())
		{
			idx = (size_t)(it - png2AssetData->tiles.begin());
			props = png2AssetData->arguments.props_default | (1 << 6);
			return true;
		}
	}

	return false;
}