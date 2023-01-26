#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include "lodepng.h"
#include "image_utils.h"

using namespace std;

int decodePNG(vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
void loadFile(vector<unsigned char>& buffer, const std::string& filename);

#define BIT(VALUE, INDEX) (1 & ((VALUE) >> (INDEX)))

#define RGBA32_SZ 4 // RGBA 8:8:8:8 is 4 bytes per pixel

enum {
	SPR_NONE,
	SPR_8x8,
	SPR_8x16,
	SPR_16x16_MSX
};

bool export_map_binary();
bool export_h_file(void);
bool export_c_file(void);

// TODO: Moved these vars to global scope (from giant main()) as start of breaking main into functions
//       They should get encapsulated
	string output_filename_h;
	string output_filename_bin;
	string output_filename_attributes_bin;
	string output_filename_tiles_bin;
	string data_name;
	//default values for some params
	int  sprite_w = 0;
	int  sprite_h = 0;
	int  pivot_x = 0xFFFFFF;
	int  pivot_y = 0xFFFFFF;
	int  pivot_w = 0xFFFFFF;
	int  pivot_h = 0xFFFFFF;
	string output_filename;
	int  bank = 0;
	bool keep_palette_order = false;
	bool output_binary = false;
	bool output_transposed = false;
	size_t max_palettes = 8;

	bool export_as_map = false;
	bool use_map_attributes = false;
	bool use_2x2_map_attributes = false;
	bool pack_map_attributes = false;
	bool convert_rgb_to_nes = false;
	size_t map_attributes_width = 0;
	size_t map_attributes_height = 0;
	size_t map_attributes_packed_width = 0;
	size_t map_attributes_packed_height = 0;
	size_t colors_per_pal;  // Number of colors per palette (ex: CGB has 4 colors per palette x 8 palettes total)
	int tile_w = 8;
	int tile_h = 16;
	int sprite_mode = SPR_8x16;

	int bpp = 2;
	unsigned int tile_origin = 0; // Default to no tile index offset

extern unsigned char rgb_to_nes[64];

struct Tile
{
	vector< unsigned char > data;
	unsigned char pal;

	Tile(size_t size = 0) : data(size), pal(0) {}
	bool operator==(const Tile& t) const
	{
		return data == t.data && pal == t.pal;
	}

	const Tile& operator=(const Tile& t)
	{
		data = t.data;
		pal = t.pal;
		return *this;
	}

	enum PackMode {
		GB,
		SGB,
		SMS,
		BPP1
	};

	vector< unsigned char > GetPackedData(PackMode pack_mode) {
		vector< unsigned char > ret((tile_w / 8) * tile_h * bpp, 0);
		if(pack_mode == GB) {
			for(int j = 0; j < tile_h; ++j) {
				for(int i = 0; i < 8; ++ i) {
					unsigned char col = data[8 * j + i];
					ret[j * 2    ] |= BIT(col, 0) << (7 - i);
					ret[j * 2 + 1] |= BIT(col, 1) << (7 - i);
				}
			}
		}
		else if(pack_mode == SGB)
		{
			for(int j = 0; j < tile_h; ++j) {
				for(int i = 0; i < 8; ++ i) {
					unsigned char col = data[8 * j + i];
					ret[j * 2    ] |= BIT(col, 0) << (7 - i);
					ret[j * 2 + 1] |= BIT(col, 1) << (7 - i);
					ret[(tile_h + j) * 2    ] |= BIT(col, 2) << (7 - i);
					ret[(tile_h + j) * 2 + 1] |= BIT(col, 3) << (7 - i);
				}
			}
		}
		else if(pack_mode == SMS)
		{
			for(int j = 0; j < tile_h; ++j) {
				for(int i = 0; i < 8; ++ i) {
					unsigned char col = data[8 * j + i];
					ret[j * 4    ] |= BIT(col, 0) << (7 - i);
					ret[j * 4 + 1] |= BIT(col, 1) << (7 - i);
					ret[j * 4 + 2] |= BIT(col, 2) << (7 - i);
					ret[j * 4 + 3] |= BIT(col, 3) << (7 - i);
				}
			}
		}
		else if(pack_mode == BPP1)
		{
			// Packs 8 pixel wide rows in order set by ExtractTile**()
			// Process all rows of pixels in the tile
			for(int j = 0; j < ((tile_w / 8) * tile_h); j++) {
				// Pack each row of 8 pixels into one byte
				for(int i = 0; i < 8; i++) {
					unsigned char col = data[8 * j + i];
					ret[j] |= BIT(col, 0) << (7 - i);
				}
			}
		}
		return ret;
	}
};

struct PNGImage
{
	vector< unsigned char > data; //data in indexed format
	unsigned int w;
	unsigned int h;

	size_t total_color_count; // Total number of colors across all palettes (palette_count x colors_per_pal)
	unsigned char* palette; //palette colors in RGBA (1 color == 4 bytes)

	unsigned char GetGBColor(int x, int y)
	{
		return data[w * y + x] % colors_per_pal;
	}


	bool ExtractGBTile(int x, int y, int tile_w, int tile_h, Tile& tile, int buffer_offset)
	{
		tile.pal = (export_as_map && !use_map_attributes) ? data[w * y + x] >> 2 : 0; //Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different

		bool all_zero = true;
		for(int j = 0; j < tile_h; ++ j)
		{
			for(int i = 0; i < tile_w; ++i)
			{
				unsigned char color_idx = GetGBColor(x + i, y + j);
				tile.data[(j * tile_w) + i + buffer_offset] = color_idx;
				all_zero = all_zero && (color_idx == 0);
			}
		}
		return !all_zero;
	}

	bool ExtractTile_MSX16x16(int x, int y, int tile_w, int tile_h, Tile& tile)
	{
		// MSX 16x16 sprite tiles are composed of four 8x8 tiles in this order UL, LL, UR, LR
		bool UL_notempty, LL_notempty, UR_notempty, LR_notempty;

		// Call these separately since otherwise some get optimized out during
		// runtime if any single one before it returns false
		UL_notempty = ExtractGBTile(x,     y,  	 8, 8, tile, 0);
		LL_notempty = ExtractGBTile(x,     y + 8, 8, 8, tile, ((8 *8) * 1));
		UR_notempty = ExtractGBTile(x + 8, y,     8, 8, tile, ((8 *8) * 2));
		LR_notempty = ExtractGBTile(x + 8, y + 8, 8, 8, tile, ((8 *8) * 3));
		return (UL_notempty || LL_notempty || UR_notempty || LR_notempty);
	}

	bool ExtractTile(int x, int y, int tile_w, int tile_h, Tile& tile, int sprite_mode)
	{
		if (sprite_mode == SPR_16x16_MSX)
			return ExtractTile_MSX16x16(x, y, tile_w, tile_h, tile);
		else
			return ExtractGBTile(x, y, tile_w, tile_h, tile, 0); // No buffer offset for normal tile extraction
	}
};

struct MTTile
{
	char offset_x;
	char offset_y;
	unsigned char offset_idx;
	unsigned char props;

	MTTile(char offset_x, char offset_y, unsigned char offset_idx, unsigned char props) : offset_x(offset_x), offset_y(offset_y), offset_idx(offset_idx), props(props) {}
	MTTile() : offset_x(0), offset_y(0), offset_idx(0), props(0) {}
};
string source_tileset;
unsigned int extra_tile_count = 0;
unsigned int source_total_color_count = 0;  // Total number of colors (palette_count x colors_per_palette)
unsigned int source_tileset_size = 0;
bool includeTileData = true;
bool includedMapOrMetaspriteData = true;
PNGImage source_tileset_image;
bool use_source_tileset = false;
bool keep_duplicate_tiles = false;

typedef vector< MTTile > MetaSprite;
vector< Tile > tiles;
vector<	MetaSprite > sprites;
vector< unsigned char > map;
vector< unsigned char > map_attributes;
PNGImage image;
int props_default = 0x00;  // Default Sprite props has no attributes enabled
bool use_structs = false;
bool flip_tiles = true;
Tile::PackMode pack_mode = Tile::GB;

Tile FlipH(const Tile& tile)
{
	Tile ret;
	for(int j = (int)tile.data.size() - 8; j >= 0; j -= 8)
	{
		for(int i = 0; i < 8; ++ i)
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
		for(int i = 7; i >= 0; -- i)
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

void GetMetaSprite(int _x, int _y, int _w, int _h, int pivot_x, int pivot_y)
{
	int last_x = _x + pivot_x;
	int last_y = _y + pivot_y;

	sprites.push_back(MetaSprite());
	MetaSprite& mt_sprite = sprites.back();
	for(int y = _y; y < _y + _h && y < (int)image.h; y += tile_h)
	{
		for(int x = _x; x < _x + _w && x < (int)image.w; x += tile_w)
		{
			Tile tile(tile_h * tile_w);
			if (image.ExtractTile(x, y, tile_w, tile_h, tile, sprite_mode))
			{
				size_t idx;
				unsigned char props;
				unsigned char pal_idx = image.data[y * image.w + x] >> 2; //We can pick the palette from the first pixel of this tile

				if(keep_duplicate_tiles)
				{
					tiles.push_back(tile);
					idx = tiles.size() - 1;
					props = props_default;
				}
				else
				{
					if(!FindTile(tile, idx, props))
					{
						if (use_source_tileset) {
							printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y,extra_tile_count+1);
							extra_tile_count++;
							includeTileData = true;
						}
						tiles.push_back(tile);
						idx = tiles.size() - 1;
						props = props_default;
					}
				}

				props |= pal_idx;

				// Scale up index based on 8x8 tiles-per-hardware sprite
				if (sprite_mode == SPR_8x16)
					idx *= 2;
				else if (sprite_mode == SPR_16x16_MSX)
					idx *= 4;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
				last_x = x;
				last_y = y;
			}
		}
	}
}

void GetMap()
{
	for(int y = 0; y < (int)image.h; y += tile_h)
	{
		for(int x = 0; x < (int)image.w; x += tile_w)
		{
			Tile tile(tile_w * tile_h);
			image.ExtractTile(x, y, tile_w, tile_h, tile, sprite_mode);

			size_t idx;
			unsigned char props;

			if(keep_duplicate_tiles)
			{
				tiles.push_back(tile);
				idx = tiles.size() - 1;
				props = props_default;
			}
			else
			{
				if(!FindTile(tile, idx, props))
				{
					if (use_source_tileset) {
						printf("found a tile not in the source tileset at %d,%d. The target tileset has %d extra tiles.\n", x, y, extra_tile_count + 1);
						extra_tile_count++;
						includeTileData = true;
					}
					tiles.push_back(tile);
					idx = tiles.size() - 1;
					props = props_default;

					if(tiles.size() > 256 && pack_mode != Tile::SMS)
						printf("Warning: found more than 256 tiles on x:%d,y:%d\n", x, y);

					if(((tiles.size() + tile_origin) > 256) && (pack_mode != Tile::SMS))
						printf("Warning: tile count (%d) + tile origin (%d) exceeds 256 at x:%d,y:%d\n", (unsigned int)tiles.size(), tile_origin, x, y);
				}
			}

			map.push_back((unsigned char)idx + tile_origin);

			if(use_map_attributes)
			{
				unsigned char pal_idx = image.data[y * image.w + x] >> bpp; //We can pick the palette from the first pixel of this tile
				if(pack_mode == Tile::SGB)
				{
					props = props << 1; //Mirror flags in SGB are on bit 7
					props |= (pal_idx + 4) << 2; //Pals are in bits 2,3,4 and need to go from 4 to 7
					map.push_back(props); //Also they are stored within the map tiles
				}
				else if(pack_mode == Tile::SMS)
				{
					props = props >> 4;
					if(idx > 255)
						props |= 1;
					map.push_back(props);
				}
				else
				{
					props |= pal_idx;
					map_attributes.push_back(props);
				}
			}

		}
	}
}
//Functor to compare entries in SetPal
struct CmpIntColor {
	bool operator() (unsigned int const& c1, unsigned int const& c2) const
	{
		unsigned char* c1_ptr = (unsigned char*)&c1;
		unsigned char* c2_ptr = (unsigned char*)&c2;

		//Compare alpha first, transparent color is considered smaller
		if(c1_ptr[0] != c2_ptr[0])
		{
			return c1_ptr[0] < c2_ptr[0];
		}
		else
		{
			// Do a compare with luminance in upper bits, and original rgb24 in lower bits. 
			// This prefers luminance, but considers RGB values for equal-luminance cases to
			// make sure the compare functor satisifed the strictly weak ordering requirement
			uint64_t lum_1 = (unsigned int)(c1_ptr[3] * 299 + c1_ptr[2] * 587 + c1_ptr[1] * 114);
			uint64_t lum_2 = (unsigned int)(c2_ptr[3] * 299 + c2_ptr[2] * 587 + c2_ptr[1] * 114);
			uint64_t rgb_1 = (c1_ptr[3] << 16) | (c1_ptr[2] << 8) | c1_ptr[1];
			uint64_t rgb_2 = (c2_ptr[3] << 16) | (c2_ptr[2] << 8) | c2_ptr[1];
			uint64_t all_1 = (lum_1 << 24) | rgb_1;
			uint64_t all_2 = (lum_2 << 24) | rgb_2;
			return all_1 > all_2;
		}
	}
};

//This set will keep colors in the palette ordered based on their grayscale values to ensure they look good on DMG
//This assumes the palette used in DMG will be 00 01 10 11
typedef set< unsigned int, CmpIntColor > SetPal;

SetPal GetPaletteColors(const PNGImage& image, int x, int y, int w, int h)
{
	SetPal ret;
	for(int j = y; j < (y + h); ++ j)
	{
		for(int i = x; i < (x + w); ++ i)
		{
			const unsigned char* color = &image.data[(j * image.w + i) * RGBA32_SZ];
			int color_int = (color[0] << 24) | (color[1] << 16) | (color[2] << 8) | color[3];
			ret.insert(color_int);
		}
	}

	for(SetPal::iterator it = ret.begin(); it != ret.end(); ++it)
	{
		if(it != ret.begin() && ((0xFF & *it) != 0xFF)) //ret.begin() should be the only one transparent
			printf("Warning: found more than one transparent color in tile at x:%d, y:%d of size w:%d, h:%d\n", x, y, w, h);
	}

	return ret;
}

unsigned int PaletteCountApplyMaxLimit(unsigned int max_palettes, unsigned int cur_palette_size)
{
	if (cur_palette_size > max_palettes)
	{
		printf("Warning: %d palettes found, truncating to %d (-max_palettes)\n", (unsigned int)cur_palette_size, (unsigned int)max_palettes);
		return max_palettes;
	}
	else
		return cur_palette_size;
}

int FindOrCreateSubPalette(const SetPal& pal, vector< SetPal >& palettes)
{
	// Return -1 if colors can't even fit in sub-palette hardware limit
	if (pal.size() > colors_per_pal)
	{
		return -1;
	}
	//Check if it matches any palettes or create a new one
	int i;
	for (i = 0; i < (int)palettes.size(); ++i)
	{
		//Try to merge this palette with any of the palettes (checking if they are equal is not enough since the palettes can have less than 4 colors)
		SetPal merged(palettes[i]);
		merged.insert(pal.begin(), pal.end());
		if (merged.size() <= colors_per_pal)
		{
			if (palettes[i].size() <= colors_per_pal)
				palettes[i] = merged; //Increase colors with this palette (it has less than 4 colors)
			return i; //Found palette
		}
	}

	if (i == (int)palettes.size())
	{
		//Palette not found, add a new one
		palettes.push_back(pal);
	}
	return i;
}


//
// Builds palettes and palette-per-tile (attributes) for an image
//
// Inputs:
//   image32         : Input image in RGBA32 format
//   palettes        : Palettes for image. Will be added-to / merged with new palettes.
//   half_resolution : If true, attributes will follow a constraint of every 2x2 cell having the same attribute.
//
// Returns: array of attributes. This always has *per-tile* dimensions, even when half_resolution is true.
//
int* BuildPalettesAndAttributes(const PNGImage& image32, vector< SetPal >& palettes, bool half_resolution)
{
	int* palettes_per_tile = new int[(image32.w / tile_w) * (image32.h / tile_h)];
	int sx = half_resolution ? 2 : 1;
	int sy = half_resolution ? 2 : 1;
	for (unsigned int y = 0; y < image32.h; y += tile_h * sy)
	{
		for (unsigned int x = 0; x < image32.w; x += tile_w * sx)
		{
			//Get palette colors on (x, y, tile_w, tile_h)
			SetPal pal = GetPaletteColors(image32, (x / sx) * sx, (y / sy) * sy, sx * tile_w, sy * tile_h);

			int subPalIndex = FindOrCreateSubPalette(pal, palettes);
			if (subPalIndex < 0)
			{
				printf("Error: more than %d colors found in tile at x:%d, y:%d of size w:%d, h:%d\n",
					(unsigned int)colors_per_pal,
					(x / sx) * sx,
					(y / sy) * sy,
					sx * tile_w,
					sy * tile_h);
				subPalIndex = 0; // Force to sub-palette 0, to allow getting a partially-incorrect output image
			}
			// Assign single or multiple entries in palettes_per_tile, to keep it independent of
			// of half_resolution parameter
			int dx = ((x / tile_w) / sx) * sx;
			int dy = ((y / tile_h) / sy) * sy;
			int w = (image32.w / tile_w);
			for (int yy = 0; yy < sy; yy++)
			{
				for (int xx = 0; xx < sx; xx++)
				{
					palettes_per_tile[(dy + yy) * w + dx + xx] = subPalIndex;
				}
			}
		}
	}
	return palettes_per_tile;
}

unsigned char GetMapAttribute(size_t x, size_t y)
{
	if (x < map_attributes_width && y < map_attributes_height)
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
	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
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
	for (size_t i = 0; i < num_vertical_nametables; i++)
	{
		bool last_nametable = (i == num_vertical_nametables - 1);
		size_t height = last_nametable ? (map_attributes_height - i * ATTRIBUTE_HEIGHT) : ATTRIBUTE_HEIGHT;
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < map_attributes_width; x++)
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
	for (size_t y = 0; y < map_attributes_packed_height; y++)
	{
		for (size_t x = 0; x < map_attributes_packed_width; x++)
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

bool GetSourceTileset(bool keep_palette_order, unsigned int max_palettes, vector< SetPal >& palettes) {

	lodepng::State sourceTilesetState;
	vector<unsigned char> buffer2;

	lodepng::load_file(buffer2, source_tileset);

	// TODO: This code block below is mostly identical (aside from memcpy, var names) to the main indexed image load.
	//       It should get deduplicated into a function.
	if (keep_palette_order) {
		//Calling with keep_palette_order means
		//-The image should be png indexed (1-8 bits per pixel)
		//-For CGB: Each 4 colors define a gbc palette, the first color is the transparent one
		//-Each rectangle with dimension(tile_w, tile_h) in the image has colors from one of those palettes only
		sourceTilesetState.info_raw.colortype = LCT_PALETTE;
		sourceTilesetState.info_raw.bitdepth = 8;

		// * Do *NOT* turn color_convert ON here.
		// When sournce PNG is indexed with bit depth was less than 8 bits per pixel then
		// color_convert may mangle the packed indexed values. Instead manually unpack them.
		//
		// This is necessary since some PNG encoders will use the minimum possible number of bits.
		//   For example 2 colors in the palette -> 1bpp -> 4 pixels per byte in the decoded image.
		//     Also see below about requirement to use palette from source image
		sourceTilesetState.decoder.color_convert = false;

		unsigned error = lodepng::decode(source_tileset_image.data, source_tileset_image.w, source_tileset_image.h, sourceTilesetState, buffer2);
		// Unpack the image if needed. Also checks and errors on incompatible palette type if needed
		if (!image_indexed_ensure_8bpp(source_tileset_image.data, source_tileset_image.w, source_tileset_image.h, (int)sourceTilesetState.info_png.color.bitdepth, (int)sourceTilesetState.info_png.color.colortype))
			return false;
		else if(error) {
			printf("decoder error %s\n", lodepng_error_text(error));
			return false;
		}

		// Use source image palette since lodepng conversion to indexed (LCT_PALETTE) won't create a palette
		// So: source_tileset_image.info_png.color.palette/size instead of source_tileset_image.info_raw.palette/size
		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, sourceTilesetState.info_png.color.palettesize / colors_per_pal);
		source_tileset_image.total_color_count = palette_count * colors_per_pal;
		source_tileset_image.palette = (uint8_t*)malloc(source_tileset_image.total_color_count * RGBA32_SZ);
		if(source_tileset_image.palette) {
			memcpy(source_tileset_image.palette, sourceTilesetState.info_png.color.palette, source_tileset_image.total_color_count * RGBA32_SZ);
		}
	}
	else {

		PNGImage image32;
		unsigned error = lodepng::decode(image32.data, image32.w, image32.h, sourceTilesetState, buffer2); //decode as 32 bit
		if (error)
		{
			printf("decoder error %s\n", lodepng_error_text(error));
			return false;
		}

		int* palettes_per_tile = BuildPalettesAndAttributes(image32, palettes, use_2x2_map_attributes);

		//Create the indexed image
		source_tileset_image.data.clear();
		source_tileset_image.w = image32.w;
		source_tileset_image.h = image32.h;

		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, palettes.size());

		source_tileset_image.total_color_count = palette_count * colors_per_pal;
		source_tileset_image.palette = new unsigned char[palette_count * colors_per_pal * RGBA32_SZ]; //colors_per_pal colors * 4 bytes each
		source_total_color_count = source_tileset_image.total_color_count;

		for (size_t p = 0; p < palette_count; ++p)
		{
			int* color_ptr = (int*)&source_tileset_image.palette[p * colors_per_pal * RGBA32_SZ];

			//TODO: if palettes[p].size() != colors_per_pal we should probably try to fill the gaps based on grayscale values

			for (SetPal::iterator it = palettes[p].begin(); it != palettes[p].end(); ++it, color_ptr++)
			{
				unsigned char* c = (unsigned char*)&(*it);
				*color_ptr = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
			}
		}

		for (size_t y = 0; y < image32.h; ++y)
		{
			for (size_t x = 0; x < image32.w; ++x)
			{
				unsigned char* c32ptr = &image32.data[(image32.w * y + x) * RGBA32_SZ];
				int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
				unsigned char palette = palettes_per_tile[(y / tile_h) * (image32.w / tile_w) + (x / tile_w)];
				unsigned char index = std::distance(palettes[palette].begin(), palettes[palette].find(color32));
				source_tileset_image.data.push_back((palette << bpp) + index);
			}
		}
	}

	// We'll change the image variable
	// So we don't have to change any of the existing code
	PNGImage temp = image;
	image = source_tileset_image;
	use_source_tileset = false;
	GetMap();

	// Our source tileset shouldn't build the map arrays up
	// Clear anything from the previous 'GetMap' call
	map.clear();
	map_attributes.clear();
	use_source_tileset = true;

	// Change the image variable back
	image = temp;

	source_tileset_size = tiles.size();

	printf("Got %d tiles from the source tileset.\n", (unsigned int)tiles.size());
	printf("Got %d palettes from the source tileset.\n", (unsigned int)(source_tileset_image.total_color_count / colors_per_pal));

	return true;

}


void Export(const PNGImage& image, const char* path)
{
	lodepng::State state;
	state.info_png.color.colortype = LCT_PALETTE;
	state.info_png.color.bitdepth = 8;
	state.info_raw.colortype = LCT_PALETTE;
	state.info_raw.bitdepth = 8;
	state.encoder.auto_convert = 0; //we specify ourselves exactly what output PNG color mode we want

#define ADD_PALETTE(R, G, B, A) lodepng_palette_add(&state.info_png.color, R, G, B, A); lodepng_palette_add(&state.info_raw, R, G, B, A)
	for(size_t p = 0; p < image.total_color_count; ++ p)
	{
		unsigned char* c = &image.palette[p * RGBA32_SZ];
		ADD_PALETTE(c[0], c[1], c[2], c[3]);
	}

	std::vector<unsigned char> buffer;
	lodepng::encode(buffer, image.data, image.w, image.h, state);
	lodepng::save_file(buffer, path);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: png2asset    <file>.png [options]\n");
		printf("-c                  ouput file (default: <png file>.c)\n");
		printf("-sw <width>         metasprites width size (default: png width)\n");
		printf("-sh <height>        metasprites height size (default: png height)\n");
		printf("-sp <props>         change default for sprite OAM property bytes (in hex) (default: 0x00)\n");
		printf("-px <x coord>       metasprites pivot x coordinate (default: metasprites width / 2)\n");
		printf("-py <y coord>       metasprites pivot y coordinate (default: metasprites height / 2)\n");
		printf("-pw <width>         metasprites collision rect width (default: metasprites width)\n");
		printf("-ph <height>        metasprites collision rect height (default: metasprites height)\n");
		printf("-spr8x8             use SPRITES_8x8\n");
		printf("-spr8x16            use SPRITES_8x16 (this is the default)\n");
		printf("-spr16x16msx        use SPRITES_16x16\n");
		printf("-b <bank>           bank (default 0)\n");
		printf("-keep_palette_order use png palette\n");
		printf("-noflip             disable tile flip\n");
		printf("-map                Export as map (tileset + bg)\n");
		printf("-use_map_attributes Use CGB BG Map attributes\n");
		printf("-use_nes_attributes Use NES BG Map attributes\n");
		printf("-use_nes_colors     Convert RGB color values to NES PPU colors\n");
		printf("-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)\n");
		printf("-bpp                bits per pixel: 1, 2, 4 (default: 2)\n");
		printf("-max_palettes       max number of palettes allowed (default: 8)\n");
		printf("                    (note: max colors = max_palettes x num colors per palette)\n");
		printf("-pack_mode          gb, sgb, sms, 1bpp (default: gb)\n");
		printf("-tile_origin        tile index offset for maps (default: 0)\n");

		printf("-tiles_only         export tile data only\n");
		printf("-maps_only          export map tilemap only\n");
		printf("-metasprites_only   export metasprite descriptors only\n");
		printf("-source_tileset     use source tileset (image with common tiles)\n");
		printf("-keep_duplicate_tiles   do not remove duplicate tiles (default: not enabled)\n");

		printf("-bin                export to binary format\n");
		printf("-transposed         export transposed (column-by-column instead of row-by-row)\n");
		return 0;
	}

	//default params
	output_filename = argv[1];
	output_filename = output_filename.substr(0, output_filename.size() - 4) + ".c";

	//Parse argv
	for(int i = 2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-sw"))
		{
			sprite_w = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-sh"))
		{
			sprite_h = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-sp"))
		{
			props_default = strtol(argv[++i], NULL, 16);
		}
		if(!strcmp(argv[i], "-px"))
		{
			pivot_x = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-py"))
		{
			pivot_y = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-pw"))
		{
			pivot_w = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-ph"))
		{
			pivot_h = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-spr8x8"))
		{
			tile_w = 8;
			tile_h = 8;
			sprite_mode = SPR_8x8;
		}
		else if(!strcmp(argv[i],"-spr8x16"))
		{
			tile_w = 8;
			tile_h = 16;
			sprite_mode = SPR_8x16;
		}
		else if(!strcmp(argv[i],"-spr16x16msx"))
		{
			tile_w = 16;
			tile_h = 16;
			sprite_mode = SPR_16x16_MSX;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			output_filename = argv[++ i];
		}
		else if(!strcmp(argv[i], "-b"))
		{
			bank = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-keep_palette_order"))
		{
			keep_palette_order = true;
		}
		else if(!strcmp(argv[i], "-noflip"))
		{
			flip_tiles = false;
		}
		else if(!strcmp(argv[i], "-map"))
		{
			export_as_map = true;
		}
		else if(!strcmp(argv[i], "-use_map_attributes"))
		{
			use_map_attributes = true;
		}
		else if (!strcmp(argv[i], "-use_nes_attributes"))
		{
			use_map_attributes = true;
			use_2x2_map_attributes = true;
			pack_map_attributes = true;
		}
		else if (!strcmp(argv[i], "-use_nes_colors"))
		{
			convert_rgb_to_nes = true;
		}
		else if(!strcmp(argv[i], "-use_structs"))
		{
			use_structs = true;
		}
		else if(!strcmp(argv[i], "-bpp"))
		{
			bpp = atoi(argv[++ i]);
		}
		else if(!strcmp(argv[i], "-max_palettes"))
		{
			max_palettes = atoi(argv[++ i]);
			if (max_palettes == 0)
			{
				printf("-max_palettes must be larger than zero\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-pack_mode"))
		{
			std::string pack_mode_str = argv[++ i];
			if     (pack_mode_str == "gb")  pack_mode = Tile::GB;
			else if(pack_mode_str == "sgb") pack_mode = Tile::SGB;
			else if(pack_mode_str == "sms") pack_mode = Tile::SMS;
			else if(pack_mode_str == "1bpp") pack_mode = Tile::BPP1;
			else
			{
				printf("-pack_mode must be one of gb, sgb, sms, 1bpp\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-tile_origin"))
		{
			tile_origin = atoi(argv[++ i]);
		}
		else if (!strcmp(argv[i], "-maps_only") || !strcmp(argv[i], "-metasprites_only"))
		{
			includeTileData = false;
		}
		else if (!strcmp(argv[i], "-tiles_only"))
		{
			includedMapOrMetaspriteData = false;
		}
		else if (!strcmp(argv[i], "-keep_duplicate_tiles"))
		{
			keep_duplicate_tiles = true;
		}
		else if (!strcmp(argv[i], "-source_tileset"))
		{
			use_source_tileset = true;
			includeTileData = false;
			source_tileset = argv[++i];
		}
		else if (!strcmp(argv[i], "-bin"))
		{
			output_binary = true;
		}
		else if (!strcmp(argv[i], "-transposed"))
		{
			output_transposed = true;
		}
	}

	colors_per_pal = 1 << bpp;

	if(export_as_map)
	{
		tile_w = 8; //Force tiles_w to 8 on maps
		tile_h = 8; //Force tiles_h to 8 on maps
		sprite_mode = SPR_NONE;
	}

	int slash_pos = (int)output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)output_filename.find_last_of('\\');
	int dot_pos = (int)output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

	output_filename_h = output_filename.substr(0, dot_pos) + ".h";
	output_filename_bin = output_filename.substr(0, dot_pos) + "_map.bin";
	output_filename_attributes_bin = output_filename.substr(0, dot_pos) + "_map_attributes.bin";
	output_filename_tiles_bin = output_filename.substr(0, dot_pos) + "_tiles.bin";
	data_name = output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(data_name.begin(), data_name.end(), '-', '_');

	// This was moved from outside the upcoming else statement when not using keep_palette_order
	// So the 'GetSourceTileset' function can pre-populate it from the source tileset
	vector< SetPal > palettes;

	if (use_source_tileset) {

		if (!GetSourceTileset(keep_palette_order, max_palettes, palettes)) {
			return 1;
		}
	}


	//load and decode png
	vector<unsigned char> buffer;
	lodepng::load_file(buffer, argv[1]);
	lodepng::State state;

	if (keep_palette_order) {
		//Calling with keep_palette_order means
		//-The image should be png indexed (1-8 bits per pixel)
		//-For CGB: Each 4 colors define a gbc palette, the first color is the transparent one
		//-Each rectangle with dimension(tile_w, tile_h) in the image has colors from one of those palettes only
		state.info_raw.colortype = LCT_PALETTE;
		state.info_raw.bitdepth = 8;

		// * Do *NOT* turn color_convert ON here.
		// When sournce PNG is indexed with bit depth was less than 8 bits per pixel then
		// color_convert may mangle the packed indexed values. Instead manually unpack them.
		//
		// This is necessary since some PNG encoders will use the minimum possible number of bits.
		//   For example 2 colors in the palette -> 1bpp -> 4 pixels per byte in the decoded image.
		//     Also see below about requirement to use palette from source image
		state.decoder.color_convert = false;

		unsigned error = lodepng::decode(image.data, image.w, image.h, state, buffer);
		// Unpack the image if needed. Also checks and errors on incompatible palette type if needed
		if (!image_indexed_ensure_8bpp(image.data, image.w, image.h, (int)state.info_png.color.bitdepth, (int)state.info_png.color.colortype))
			return 1;
		else if(error) {
			printf("decoder error %s\n", lodepng_error_text(error));
			return 1;
		}

		// Use source image palette since lodepng conversion to indexed (LCT_PALETTE) won't create a palette
		// So: state.info_png.color.palette/size instead of state.info_raw.palette/size
		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, state.info_png.color.palettesize / colors_per_pal);
		image.total_color_count = palette_count * colors_per_pal;
		image.palette = state.info_png.color.palette;

		// TODO: Enable dimension check
		// // Validate image dimensions
		// if (((image.w % tile_w) != 0) || ((image.h % tile_h) != 0))
		// {
		// 	printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image.w, image.h, tile_w, tile_h);
		// 	return 1;
		// }

		if (use_source_tileset) {

			// Make sure these two values match when keeping palette order
			if (image.total_color_count != source_tileset_image.total_color_count) {

				printf("error: The number of color palette's for your source tileset (%d) and target image (%d) do not match.", (unsigned int)source_tileset_image.total_color_count, (unsigned int)image.total_color_count);
				return 1;
			}

			size_t size = max(image.total_color_count, source_tileset_image.total_color_count);

			// Make sure these two values match when keeping palette order
			if (memcmp(image.palette, source_tileset_image.palette, size) != 0) {

				printf("error: The palette's for your source tileset and target image do not match.");
				return 1;
			}
		}
	}
	else
	{
		PNGImage image32;
		unsigned error = lodepng::decode(image32.data, image32.w, image32.h, state, buffer); //decode as 32 bit
		if(error)
		{
			printf("decoder error %s\n", lodepng_error_text(error));
			return 1;
		}

		// Validate image dimensions
		if( ((image32.w % tile_w) != 0) || ((image32.h % tile_h) != 0) )
		{
			printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image32.w, image32.h, tile_w, tile_h);
			return 1;
		}

		int* palettes_per_tile = BuildPalettesAndAttributes(image32, palettes, use_2x2_map_attributes);

		//Create the indexed image
		image.data.clear();
		image.w = image32.w;
		image.h = image32.h;

		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, palettes.size());

		image.total_color_count = palette_count * colors_per_pal;
		image.palette = new unsigned char[palette_count * colors_per_pal * RGBA32_SZ]; // total color count * 4 bytes each

		// If we are using a sourcetileset and have more palettes than it defines
		if (use_source_tileset && (image.total_color_count > source_total_color_count)) {
			printf("Found %d extra palette(s) for target tilemap.\n", (unsigned int)((image.total_color_count - source_total_color_count) / colors_per_pal));
		}
		for(size_t p = 0; p < palette_count; ++p)
		{
			int *color_ptr = (int*)&image.palette[p * colors_per_pal * RGBA32_SZ];

			//TODO: if palettes[p].size() != colors_per_pal we should probably try to fill the gaps based on grayscale values

			for(SetPal::iterator it = palettes[p].begin(); it != palettes[p].end(); ++ it, color_ptr ++)
			{
				unsigned char* c = (unsigned char*)&(*it);
				*color_ptr = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
			}
		}

		for(size_t y = 0; y < image32.h; ++ y)
		{
			for(size_t x = 0; x < image32.w; ++x)
			{
				unsigned char* c32ptr = &image32.data[(image32.w * y + x) * RGBA32_SZ];
				int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
				unsigned char palette = palettes_per_tile[(y / tile_h) * (image32.w / tile_w) + (x / tile_w)];
				unsigned char index = std::distance(palettes[palette].begin(), palettes[palette].find(color32));
				image.data.push_back((palette << bpp) + index);
			}
		}

		//Test: output png to see how it looks
		//Export(image, "temp.png");
	}

	if(sprite_w == 0) sprite_w = (int)image.w;
	if(sprite_h == 0) sprite_h = (int)image.h;
	if(pivot_x == 0xFFFFFF) pivot_x = sprite_w / 2;
	if(pivot_y == 0xFFFFFF) pivot_y = sprite_h / 2;
	if(pivot_w == 0xFFFFFF) pivot_w = sprite_w;
	if(pivot_h == 0xFFFFFF) pivot_h = sprite_h;

	if(export_as_map)
	{
		//Extract map
		GetMap();
	}
	else
	{
		//Extract metasprites
		for(int y = 0; y < (int)image.h; y += sprite_h)
		{
			for(int x = 0; x < (int)image.w; x += sprite_w)
			{
				GetMetaSprite(x, y, sprite_w, sprite_h, pivot_x, pivot_y);
			}
		}
	}
	map_attributes_width = image.w / 8;
	map_attributes_height = image.h / 8;
	// Optionally perform 2x2 reduction on attributes (NES attribute table has this format)
	if(use_2x2_map_attributes)
	{
		// NES attribute map dimensions are half-resolution 
		ReduceMapAttributes2x2(palettes);
	}
	// Optionally align and pack map attributes into NES PPU format
	if (pack_map_attributes)
	{
		AlignMapAttributes();
		PackMapAttributes();
	}
	else
	{
		// Use original attribute dimensions for packed
		map_attributes_packed_width = map_attributes_width;
		map_attributes_packed_height = map_attributes_height;
	}

	// === EXPORT ===

	// Header file export
	if (export_h_file() == false) return 1; // Exit with Fail

	if ((export_as_map) && (output_binary)) {
		// Handle special case of binary map export
		export_map_binary();
	} else {
		// Normal source file export
		if (export_c_file() == false) return 1; // Exit with Fail
	}
}


bool export_h_file(void) {

	FILE* file;

	file=fopen(output_filename_h.c_str(), "w");
	if (!file) {
		printf("Error writing file: %s", output_filename_h.c_str() );
		return false;
	}

	fprintf(file, "//AUTOGENERATED FILE FROM png2asset\n");
	fprintf(file, "#ifndef METASPRITE_%s_H\n", data_name.c_str());
	fprintf(file, "#define METASPRITE_%s_H\n", data_name.c_str());
	fprintf(file, "\n");
	fprintf(file, "#include <stdint.h>\n");
	fprintf(file, "#include <gbdk/platform.h>\n");
	fprintf(file, "#include <gbdk/metasprites.h>\n");
	fprintf(file, "\n");
	if(use_structs)
	{
		if(export_as_map)
		{
			fprintf(file, "#include \"TilesInfo.h\"\n");
			fprintf(file, "#include \"MapInfo.h\"\n");
			fprintf(file, "\n");
			fprintf(file, "extern const struct TilesInfo %s_tiles_info;\n", data_name.c_str());
			fprintf(file, "extern const struct MapInfo %s;\n", data_name.c_str());
		}
		else
		{
			fprintf(file, "#include \"MetaSpriteInfo.h\"\n");
			fprintf(file, "\n");
			fprintf(file, "extern const struct MetaSpriteInfo %s;\n", data_name.c_str());
		}
	}
	else
	{
		fprintf(file, "#define %s_TILE_ORIGIN %d\n", data_name.c_str(), tile_origin);
		fprintf(file, "#define %s_TILE_W %d\n", data_name.c_str(), tile_w);
		fprintf(file, "#define %s_TILE_H %d\n", data_name.c_str(), tile_h);
		fprintf(file, "#define %s_WIDTH %d\n",  data_name.c_str(), sprite_w);
		fprintf(file, "#define %s_HEIGHT %d\n", data_name.c_str(), sprite_h);
		// The TILE_COUNT calc here is referring to number of 8x8 tiles,
		// so the >> 3 for each sizes axis is to get a multiplier for larger hardware sprites such as 8x16 and 16x16
		fprintf(file, "#define %s_TILE_COUNT %d\n", data_name.c_str(), ((unsigned int)tiles.size() - source_tileset_size) * (tile_h >> 3) * (tile_w >> 3));
		fprintf(file, "#define %s_PALETTE_COUNT %d\n", data_name.c_str(), (unsigned int)(image.total_color_count / colors_per_pal));
		fprintf(file, "#define %s_COLORS_PER_PALETTE %d\n", data_name.c_str(), (unsigned int)colors_per_pal);
		fprintf(file, "#define %s_TOTAL_COLORS %d\n", data_name.c_str(), (unsigned int)image.total_color_count);

		if (includedMapOrMetaspriteData) {

			if(export_as_map)
			{
				fprintf(file, "#define %s_MAP_ATTRIBUTES ",  data_name.c_str());
				if(use_map_attributes && map_attributes.size())
					fprintf(file, "%s_map_attributes\n", data_name.c_str());
				else
					fprintf(file, "0\n");

				if (use_map_attributes)
				{
					int scale = use_2x2_map_attributes ? 2 : 1;
					fprintf(file, "#define %s_MAP_ATTRIBUTES_WIDTH %d\n", data_name.c_str(), (int)(scale * map_attributes_width));
					fprintf(file, "#define %s_MAP_ATTRIBUTES_HEIGHT %d\n", data_name.c_str(), (int)(scale * map_attributes_height));
					fprintf(file, "#define %s_MAP_ATTRIBUTES_PACKED_WIDTH %d\n", data_name.c_str(), (int)map_attributes_packed_width);
					fprintf(file, "#define %s_MAP_ATTRIBUTES_PACKED_HEIGHT %d\n", data_name.c_str(), (int)map_attributes_packed_height);
				}

				if(use_structs)
				{
					fprintf(file, "#define %s_TILE_PALS ",  data_name.c_str());
					if(use_map_attributes)
						fprintf(file, "0\n");
					else
						fprintf(file, "%s_tile_pals\n", data_name.c_str());
				}
			}
			else
			{
				fprintf(file, "#define %s_PIVOT_X %d\n", data_name.c_str(), pivot_x);
				fprintf(file, "#define %s_PIVOT_Y %d\n", data_name.c_str(), pivot_y);
				fprintf(file, "#define %s_PIVOT_W %d\n", data_name.c_str(), pivot_w);
				fprintf(file, "#define %s_PIVOT_H %d\n", data_name.c_str(), pivot_h);
			}
		}
		fprintf(file, "\n");
		fprintf(file, "BANKREF_EXTERN(%s)\n", data_name.c_str());
		fprintf(file, "\n");

		// If we are not using a source tileset, or if we have extra palettes defined
		if (image.total_color_count - source_total_color_count > 0 || !use_source_tileset) {
			fprintf(file, "extern const palette_color_t %s_palettes[%d];\n", data_name.c_str(), (unsigned int)image.total_color_count - source_total_color_count);
		}
		if (includeTileData) {
			fprintf(file, "extern const uint8_t %s_tiles[%d];\n", data_name.c_str(), (unsigned int)((tiles.size() - source_tileset_size) * (tile_w * tile_h * bpp / 8)));
		}

		fprintf(file, "\n");
		if (includedMapOrMetaspriteData) {
			if(export_as_map)
			{
				fprintf(file, "extern const unsigned char %s_map[%d];\n", data_name.c_str(), (unsigned int)map.size());

				if(use_map_attributes && map_attributes.size()) {
						fprintf(file, "extern const unsigned char %s_map_attributes[%d];\n", data_name.c_str(), (unsigned int)map_attributes.size());
				}
				else
				{
					// Some platforms (like SMS/GG) encode attributes as part of map
					// For compatibility, add a define that makes _map_attributes equal _map,
					// so that set_bkg_attributes can work the same on these platforms
					fprintf(file, "#define %s_map_attributes %s_map\n", data_name.c_str(), data_name.c_str());
				}
				if (!use_map_attributes && (includeTileData) && (use_structs)) {
					fprintf(file, "extern const unsigned char* %s_tile_pals[%d];\n", data_name.c_str(), (unsigned int)tiles.size());
				}
			}
			else
			{
				fprintf(file, "extern const metasprite_t* const %s_metasprites[%d];\n", data_name.c_str(), (unsigned int)sprites.size());
			}
		}
	}
	fprintf(file, "\n");
	fprintf(file, "#endif\n");


	fclose(file);
	// END: Output .h FILE

	return true; // success
}



bool export_c_file(void) {

	FILE* file;

	file = fopen(output_filename.c_str(), "w");
	if(!file) {
		printf("Error writing file: %s", output_filename.c_str() );
		return false;
	}

	if (bank) fprintf(file, "#pragma bank %d\n\n", bank);

	fprintf(file, "//AUTOGENERATED FILE FROM png2asset\n\n");

	fprintf(file, "#include <stdint.h>\n");
	fprintf(file, "#include <gbdk/platform.h>\n");
	fprintf(file, "#include <gbdk/metasprites.h>\n");
	fprintf(file, "\n");

	fprintf(file, "BANKREF(%s)\n\n", data_name.c_str());

	// Are we not using a source tileset, or do we have extra colors
	if (image.total_color_count - source_total_color_count > 0||!use_source_tileset) {

			// Subtract however many palettes we had in the source tileset
			fprintf(file, "const palette_color_t %s_palettes[%d] = {\n", data_name.c_str(), (unsigned int)image.total_color_count - source_total_color_count);

			// Offset by however many palettes we had in the source tileset
			for (size_t i = source_total_color_count / colors_per_pal; i < image.total_color_count / colors_per_pal; ++i)
			{
				if(i != 0)
					fprintf(file, ",\n");
				fprintf(file, "\t");

				unsigned char* pal_ptr = &image.palette[i * (colors_per_pal * RGBA32_SZ)];
				for(int c = 0; c < (int)colors_per_pal; ++ c, pal_ptr += RGBA32_SZ)
				{
					size_t rgb222 = (((pal_ptr[2] >> 6) & 0x3) << 4) |
									(((pal_ptr[1] >> 6) & 0x3) << 2) |
									(((pal_ptr[0] >> 6) & 0x3) << 0);
					if (convert_rgb_to_nes)
						fprintf(file, "0x%0X", rgb_to_nes[rgb222]);
					else
						fprintf(file, "RGB8(%3d,%3d,%3d)", pal_ptr[0], pal_ptr[1], pal_ptr[2]);
					if(c != (int)colors_per_pal - 1)
						fprintf(file, ", ");
					 // Line break every 4 color entries, to keep line width down
					if (((c + 1) % 4) == 0)
						fprintf(file, "\n\t");
				}
			}
			fprintf(file, "\n};\n");
	}

	if (includeTileData) {
		fprintf(file, "\n");
		fprintf(file, "const uint8_t %s_tiles[%d] = {\n", data_name.c_str(), (unsigned int)((tiles.size()-source_tileset_size) * tile_w * tile_h * bpp / 8));
		fprintf(file, "\t");
		for (vector< Tile >::iterator it = tiles.begin()+ source_tileset_size; it != tiles.end(); ++it)
		{

			int line_break = 1; // Start with 1 to prevent line break on first iteration
			vector< unsigned char > packed_data = (*it).GetPackedData(pack_mode);
			for(vector< unsigned char >::iterator it2 = packed_data.begin(); it2 != packed_data.end(); ++it2)
			{
				fprintf(file, "0x%02x", (*it2));
				if((it + 1) != tiles.end() || (it2 + 1) != packed_data.end())
					fprintf(file, ",");
				// Add a line break after each 8x8 tile
				if (((line_break++) % (8 / bpp)) == 0)
					fprintf(file, "\n\t");
			}

			if ((!export_as_map) && (it != tiles.end()))
				fprintf(file, "\n");
		}
		fprintf(file, "};\n\n");
	}

	if(includedMapOrMetaspriteData) {

		if(!export_as_map)
		{
			for(vector< MetaSprite >::iterator it = sprites.begin(); it != sprites.end(); ++ it)
			{
				fprintf(file, "const metasprite_t %s_metasprite%d[] = {\n", data_name.c_str(), (int)(it - sprites.begin()));
				for(MetaSprite::iterator it2 = (*it).begin(); it2 != (*it).end(); ++ it2)
				{                    
					int pal_idx = (*it2).props & 0xF;
                    int flip_x = ((*it2).props >> 5) & 1;
                    int flip_y = ((*it2).props >> 6) & 1;
					fprintf(file,
					        "\tMETASPR_ITEM(%d, %d, %d, S_PAL(%d)%s%s),\n",
					        (*it2).offset_y,
					        (*it2).offset_x,
					        (*it2).offset_idx,
					        pal_idx,
					        flip_x ? " | S_FLIPX" : "",
					        flip_y ? " | S_FLIPY" : "");
				}
				fprintf(file, "\tMETASPR_TERM\n");
				fprintf(file, "};\n\n");
			}

			fprintf(file, "const metasprite_t* const %s_metasprites[%d] = {\n\t", data_name.c_str(), (unsigned int)sprites.size());
			for(vector< MetaSprite >::iterator it = sprites.begin(); it != sprites.end(); ++ it)
			{
				fprintf(file, "%s_metasprite%d", data_name.c_str(), (int)(it - sprites.begin()));
				if(it + 1 != sprites.end())
					fprintf(file, ", ");
			}
			fprintf(file, "\n};\n");

			if(use_structs)
			{
				fprintf(file, "\n");
				fprintf(file, "#include \"MetaSpriteInfo.h\"\n");
				fprintf(file, "const struct MetaSpriteInfo %s = {\n", data_name.c_str());
				fprintf(file, "\t%d, //width\n", pivot_w);
				fprintf(file, "\t%d, //height\n", pivot_h);
				fprintf(file, "\t%d, //num tiles\n", (unsigned int)tiles.size() * (tile_h >> 3));
				fprintf(file, "\t%s_tiles, //tiles\n", data_name.c_str());
				fprintf(file, "\t%d, //num palettes\n", (unsigned int)(image.total_color_count / colors_per_pal));
				fprintf(file, "\t%s_palettes, //CGB palette\n", data_name.c_str());
				fprintf(file, "\t%d, //num sprites\n", (unsigned int)sprites.size());
				fprintf(file, "\t%s_metasprites, //metasprites\n", data_name.c_str());
				fprintf(file, "};\n");
			}
		}
		else
		{
			if (includeTileData) {
				if(use_structs)
				{
					//Export tiles pals (if any)
					if(!use_map_attributes)
					{
						fprintf(file, "\n");
						fprintf(file, "const uint8_t %s_tile_pals[%d] = {\n\t", data_name.c_str(), (unsigned int)tiles.size()- source_tileset_size);
						for(vector< Tile >::iterator it = tiles.begin()+ source_tileset_size; it != tiles.end(); ++ it)
						{
							if(it != tiles.begin())
								fprintf(file, ", ");
							fprintf(file, "%d", it->pal);
						}
						fprintf(file, "\n};\n");
					}
					//Export Tiles Info
					fprintf(file, "\n");
					fprintf(file, "#include \"TilesInfo.h\"\n");
					fprintf(file, "BANKREF(%s_tiles_info)\n", data_name.c_str());
					fprintf(file, "const struct TilesInfo %s_tiles_info = {\n", data_name.c_str());
					fprintf(file, "\t%d, //num tiles\n", (unsigned int)tiles.size() * (tile_h >> 3));
					fprintf(file, "\t%s_tiles, //tiles\n", data_name.c_str());
					fprintf(file, "\t%d, //num palettes\n", (unsigned int)(image.total_color_count / colors_per_pal));
					fprintf(file, "\t%s_palettes, //palettes\n", data_name.c_str());
					if(!use_map_attributes)
						fprintf(file, "\t%s_tile_pals, //tile palettes\n", data_name.c_str());
					else
						fprintf(file, "\t0 //tile palettes\n");
					fprintf(file, "};\n");
				}
			}

			//Export map
			fprintf(file, "\n");
			fprintf(file, "const unsigned char %s_map[%d] = {\n", data_name.c_str(), (unsigned int)map.size());
			size_t line_size = map.size() / (image.h / 8);
			if (output_transposed) {

				for(size_t i = 0; i < line_size; ++i)
				{
					fprintf(file, "\t");
					for (size_t j = 0; j < image.h / 8; ++j)
					{
						fprintf(file, "0x%02x,", map[j * line_size + i]);
					}
					fprintf(file, "\n");
				}
			}
			else {

				for (size_t j = 0; j < image.h / 8; ++j)
				{
					fprintf(file, "\t");
					for (size_t i = 0; i < line_size; ++i)
					{
						fprintf(file, "0x%02x,", map[j * line_size + i]);
					}
					fprintf(file, "\n");
				}
			}
			fprintf(file, "};\n");


			//Export map attributes (if any)
			if(use_map_attributes && map_attributes.size())
			{
				fprintf(file, "\n");
				fprintf(file, "const unsigned char %s_map_attributes[%d] = {\n", data_name.c_str(), (unsigned int)map_attributes.size());
				if (output_transposed) {
					for (size_t i = 0; i < map_attributes_packed_width; ++i)
					{
						fprintf(file, "\t");
						for (size_t j = 0; j < map_attributes_packed_height; ++j)
						{
							fprintf(file, "0x%02x,", map_attributes[j * map_attributes_packed_width + i]);
						}
						fprintf(file, "\n");
					}
				}
				else {
					for (size_t j = 0; j < map_attributes_packed_height; ++j)
					{
						fprintf(file, "\t");
						for (size_t i = 0; i < map_attributes_packed_width; ++i)
						{
							fprintf(file, "0x%02x,", map_attributes[j * map_attributes_packed_width + i]);
						}
						fprintf(file, "\n");
					}
				}

				fprintf(file, "};\n");
			}

			if(use_structs)
			{
				//Export Map Info
				fprintf(file, "\n");
				fprintf(file, "#include \"MapInfo.h\"\n");
				fprintf(file, "BANKREF_EXTERN(%s_tiles_info)\n", data_name.c_str());
				fprintf(file, "const struct MapInfo %s = {\n", data_name.c_str());
				fprintf(file, "\t%s_map, //map\n", data_name.c_str());
				fprintf(file, "\t%d, //with\n", image.w >> 3);
				fprintf(file, "\t%d, //height\n", image.h >> 3);
				if (use_map_attributes && map_attributes.size())
					fprintf(file, "\t%s_map_attributes, //map attributes\n", data_name.c_str());
				else
					fprintf(file, "\t%s, //map attributes\n", "0");
				fprintf(file, "\tBANK(%s_tiles_info), //tiles bank\n", data_name.c_str());
				fprintf(file, "\t&%s_tiles_info, //tiles info\n", data_name.c_str());
				fprintf(file, "};\n");
			}
		}
	}

	fclose(file);

	return true; // success
}


bool export_map_binary() {

		std::ofstream mapBinaryFile, mapAttributesBinaryfile,tilesBinaryFile;
		mapBinaryFile.open(output_filename_bin, std::ios_base::binary);
		tilesBinaryFile.open(output_filename_tiles_bin, std::ios_base::binary);

		for (vector< Tile >::iterator it = tiles.begin() + source_tileset_size; it != tiles.end(); ++it)
		{

			vector< unsigned char > packed_data = (*it).GetPackedData(pack_mode);
			for (vector< unsigned char >::iterator it2 = packed_data.begin(); it2 != packed_data.end(); ++it2)
			{

				const char chars[] = { (const char)(*it2) };
				tilesBinaryFile.write(chars, 1);
			}

		}


		// Open our file for writing attributes if specified
		if (use_map_attributes)mapAttributesBinaryfile.open(output_filename_attributes_bin, std::ios_base::binary);

		int columns = image.w >> 3;
		int rows = image.h >> 3;

		// If we want the values to be column-by-column
		if (output_transposed) {

			// Swap the column/row for loops
			for (int column = 0; column < columns; column++) {
				for (int row = 0; row < rows; ++row) {

					int tile = column + row * columns;

					const char mapChars[] = { (const char)map[tile] };

					// Write map items column-by-column
					mapBinaryFile.write(mapChars, 1);
					if(use_map_attributes) {
						const char mapAttributeChars[] = { (const char)map_attributes[tile] };
						mapAttributesBinaryfile.write(mapAttributeChars, 1);
					}
				}
			}
		}
		else {

			// Write the arrays as-is, row-by-row
			mapBinaryFile.write((const char*)(&map[0]), rows * columns);
			if (use_map_attributes)mapAttributesBinaryfile.write((const char*)(&map_attributes[0]), rows * columns);
		}

		// Finalize the files
		mapBinaryFile.close();
		tilesBinaryFile.close();
		if (use_map_attributes)mapAttributesBinaryfile.close();

	return true; // success
}
