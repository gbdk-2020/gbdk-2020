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


using namespace std;

#include "png2asset.h"
#include "image_utils.h"


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

bool FindTile(const Tile& t, size_t& idx, unsigned char& props)
{
	vector< Tile >::iterator it;
	it = find(tiles.begin(), tiles.end(), t);
	if(it != tiles.end())
	{
		idx = (size_t)(it - tiles.begin());
		props = props_default;
		return true;
	}

	if(flip_tiles)
	{
		Tile tile = FlipV(t);
		it = find(tiles.begin(), tiles.end(), tile);
		if(it != tiles.end())
		{
			idx = (size_t)(it - tiles.begin());
			props = props_default | (1 << 5);
			return true;
		}

		tile = FlipH(tile);
		it = find(tiles.begin(), tiles.end(), tile);
		if(it != tiles.end())
		{
			idx = (size_t)(it - tiles.begin());
			props = props_default | (1 << 5) | (1 << 6);
			return true;
		}

		tile = FlipV(tile);
		it = find(tiles.begin(), tiles.end(), tile);
		if(it != tiles.end())
		{
			idx = (size_t)(it - tiles.begin());
			props = props_default | (1 << 6);
			return true;
		}
	}

	return false;
}