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

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"

unsigned char GetMapAttribute(size_t x, size_t y)
{
	if(x < map_attributes_width && y < map_attributes_height)
		return map_attributes[y * map_attributes_width + x];
	else
		return 0;
}

void ReduceMapAttributes2x2(const vector< SetPal >& palettes)
{
	size_t w = (map_attributes_width + 1) / 2;
	size_t h = (map_attributes_height + 1) / 2;
	vector< unsigned char > map_attributes_2x2;
	map_attributes_2x2.resize(w * h);
	for(size_t y = 0; y < h; y++)
	{
		for(size_t x = 0; x < w; x++)
		{
			// Use only Top-left attribute, ignoring the other three as they should now be identical
			map_attributes_2x2[y * w + x] = GetMapAttribute(2 * x, 2 * y);
		}
	}
	// Overwrite old attributes
	map_attributes_width = w;
	map_attributes_height = h;
	map_attributes = map_attributes_2x2;
}

//
// Aligns map attribute data to be aligned properly for NES and set_bkg_submap_attributes
// Namely:
// * Width aligned to multiples of 2 to reflect the NES's packed attribute table
// * Every 16th row is blank to reflect the unused row in the NES's packed attribute table
//
void AlignMapAttributes()
{
	const size_t ATTRIBUTE_HEIGHT = 15;
	const size_t ATTRIBUTE_ALIGNED_HEIGHT = 16;
	vector< unsigned char > map_attributes_aligned;
	size_t map_attributes_aligned_width = 2 * ((map_attributes_width + 1) / 2);
	size_t num_vertical_nametables = (map_attributes_height + ATTRIBUTE_HEIGHT - 1) / ATTRIBUTE_HEIGHT;
	map_attributes_aligned.resize(map_attributes_aligned_width * (num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT));
	for(size_t i = 0; i < num_vertical_nametables; i++)
	{
		bool last_nametable = (i == num_vertical_nametables - 1);
		size_t height = last_nametable ? (map_attributes_height - i * ATTRIBUTE_HEIGHT) : ATTRIBUTE_HEIGHT;
		for(size_t y = 0; y < height; y++)
		{
			for(size_t x = 0; x < map_attributes_width; x++)
			{
				map_attributes_aligned[(i * ATTRIBUTE_ALIGNED_HEIGHT + y) * map_attributes_aligned_width + x] =
					map_attributes[(i * ATTRIBUTE_HEIGHT + y) * map_attributes_width + x];
			}
		}
	}
	// Overwrite old attributes
	map_attributes_width = map_attributes_aligned_width;
	map_attributes_height = num_vertical_nametables * ATTRIBUTE_ALIGNED_HEIGHT;
	map_attributes = map_attributes_aligned;
}

//
// Pack map attributes
// (NES packs multiple 2-bit entries into one byte)
//
void PackMapAttributes()
{
	vector< unsigned char > map_attributes_packed;
	map_attributes_packed_width = (map_attributes_width + 1) / 2;
	map_attributes_packed_height = (map_attributes_height + 1) / 2;
	map_attributes_packed.resize(map_attributes_packed_width * map_attributes_packed_height);
	for(size_t y = 0; y < map_attributes_packed_height; y++)
	{
		for(size_t x = 0; x < map_attributes_packed_width; x++)
		{
			unsigned char a_tl = GetMapAttribute(2 * x + 0, 2 * y + 0);
			unsigned char a_tr = GetMapAttribute(2 * x + 1, 2 * y + 0);
			unsigned char a_bl = GetMapAttribute(2 * x + 0, 2 * y + 1);
			unsigned char a_br = GetMapAttribute(2 * x + 1, 2 * y + 1);
			unsigned char packed_bits = (a_br << 6) | (a_bl << 4) | (a_tr << 2) | (a_tl << 0);
			map_attributes_packed[map_attributes_packed_width * y + x] = packed_bits;
		}
	}
	// Overwrite old attributes
	map_attributes = map_attributes_packed;
}