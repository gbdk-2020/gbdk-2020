#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include "lodepng.h"

using namespace std;

int decodePNG(vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
void loadFile(vector<unsigned char>& buffer, const std::string& filename);

#define BIT(VALUE, INDEX) (1 & ((VALUE) >> (INDEX)))

bool export_as_map = false;
bool use_map_attributes = false;
size_t pal_size;
#define TILE_W 8
int tile_h;
int bpp = 2;
unsigned int tile_origin = 0; // Default to no tile index offset

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
	};

	vector< unsigned char > GetPackedData(PackMode pack_mode) {
		vector< unsigned char > ret(tile_h * bpp, 0);
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
		return ret;
	}
};

struct PNGImage
{
	vector< unsigned char > data; //data in indexed format
	unsigned int w;
	unsigned int h;

	size_t palettesize; //number of palette colors
	unsigned char* palette; //palette colors in RGBA (1 color == 4 bytes)

	unsigned char GetGBColor(int x, int y)
	{
		return data[w * y + x] % pal_size;
	}

	bool ExtractGBTile(int x, int y, int tile_h, Tile& tile)
	{
		tile.pal = (export_as_map && !use_map_attributes) ? data[w * y + x] >> 2 : 0; //Set the palette to 0 when pals are not stored in tiles to allow tiles to be equal even when their palettes are different

		bool all_zero = true;
		for(int j = 0; j < tile_h; ++ j)
		{
			for(int i = 0; i < 8; ++i)
			{
				unsigned char color_idx = GetGBColor(x + i, y + j);
				tile.data[j * 8 + i] = color_idx;
				all_zero = all_zero && (color_idx == 0);
			}
		}
		return !all_zero;
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
		for(int x = _x; x < _x + _w && x < (int)image.w; x += 8)
		{
			Tile tile(tile_h * 8);
			if (image.ExtractGBTile(x, y, tile_h, tile))
			{
				size_t idx;
				unsigned char props;
				unsigned char pal_idx = image.data[y * image.w + x] >> 2; //We can pick the palette from the first pixel of this tile
				if(!FindTile(tile, idx, props))
				{
					tiles.push_back(tile);
					idx = tiles.size() - 1;
					props = props_default;
				}

				props |= pal_idx;

				if(tile_h == 16)
					idx *= 2;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, (unsigned char)idx, props));
				
				last_x = x;
				last_y = y;
			}
		}
	}
}

void GetMap()
{
	for(int y = 0; y < (int)image.h; y += 8)
	{
		for(int x = 0; x < (int)image.w; x += 8)
		{
			Tile tile(8 * 8);
			image.ExtractGBTile(x, y, 8, tile);
			
			size_t idx;
			unsigned char props;
			if(!FindTile(tile, idx, props))
			{
				tiles.push_back(tile);
				idx = tiles.size() - 1;
				props = props_default;

				if(tiles.size() > 256 && pack_mode != Tile::SMS)
					printf("Warning: found more than 256 tiles on x:%d,y:%d\n", x, y);

				if(((tiles.size() + tile_origin) > 256) && (pack_mode != Tile::SMS))
					printf("Warning: tile count (%d) + tile origin (%d) exceeds 256 at x:%d,y:%d\n", (unsigned int)tiles.size(), tile_origin, x, y);
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
			unsigned int lum_1 = (unsigned int)(c1_ptr[3] * 0.299f + c1_ptr[2] * 0.587f + c1_ptr[1] * 0.114f);
			unsigned int lum_2 = (unsigned int)(c2_ptr[3] * 0.299f + c2_ptr[2] * 0.587f + c2_ptr[1] * 0.114f);
			return lum_1 > lum_2;
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
			const unsigned char* color = &image.data[(j * image.w + i) * 4];
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

void Export(const PNGImage& image, const char* path)
{
	lodepng::State state;
	state.info_png.color.colortype = LCT_PALETTE; 
	state.info_png.color.bitdepth = 8;
	state.info_raw.colortype = LCT_PALETTE;
	state.info_raw.bitdepth = 8;
	state.encoder.auto_convert = 0; //we specify ourselves exactly what output PNG color mode we want

#define ADD_PALETTE(R, G, B, A) lodepng_palette_add(&state.info_png.color, R, G, B, A); lodepng_palette_add(&state.info_raw, R, G, B, A)
	for(size_t p = 0; p < image.palettesize; ++ p)
	{
		unsigned char* c = &image.palette[p * 4];
		ADD_PALETTE(c[0], c[1], c[2], c[3]);
	}
				
	std::vector<unsigned char> buffer;
	lodepng::encode(buffer, image.data, image.w, image.h, state);
	lodepng::save_file(buffer, path);
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: png2asset    <file>.png [options]\n");
		printf("-c                  ouput file (default: <png file>.c)\n");
		printf("-sw <width>         metasprites width size (default: png width)\n");
		printf("-sh <height>        metasprites height size (default: png height)\n");
		printf("-sp <props>         change default for sprite OAM property bytes (in hex) (default: 0x00)\n");
		printf("-px <x coord>       metasprites pivot x coordinate (default: metasprites width / 2)\n");
		printf("-py <y coord>       metasprites pivot y coordinate (default: metasprites height / 2)\n");
		printf("-pw <width>         metasprites collision rect widht (default: metasprites width)\n");
		printf("-ph <height>        metasprites collision rect height (default: metasprites height)\n");
		printf("-spr8x8             use SPRITES_8x8 (default: SPRITES_8x16)\n");
		printf("-spr8x16            use SPRITES_8x16 (default: SPRITES_8x16)\n");
		printf("-b <bank>           bank (default 0)\n");
		printf("-keep_palette_order use png palette\n");
		printf("-noflip             disable tile flip\n");
		printf("-map                Export as map (tileset + bg)\n");
		printf("-use_map_attributes Use CGB BG Map attributes (default: palettes are stored for each tile in a separate array)\n");
		printf("-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)\n");
		printf("-bpp                bits per pixel: 2, 4 (default: 2)\n");
		printf("-max_palettes       maximum number of palettes allowed (default: 2)\n");
		printf("-pack_mode          gb, sgb or sms (default:GB)\n");
		printf("-tile_origin        tile index offset for maps (instead of zero)\n");
		return 0;
	}

	//default params
	int sprite_w = 0;
	int sprite_h = 0;
	int pivot_x = 0xFFFFFF;
	int pivot_y = 0xFFFFFF;
	int pivot_w = 0xFFFFFF;
	int pivot_h = 0xFFFFFF;
	tile_h = 16;
	string output_filename = argv[1];
	output_filename = output_filename.substr(0, output_filename.size() - 4) + ".c";
	int bank = 0;
	bool keep_palette_order = false;
	size_t max_palettes = 8;

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
			props_default = strtol(argv[++ i], NULL, 16);
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
			tile_h = 8;
		}
		else if(!strcmp(argv[i],"-spr8x16"))
		{
			tile_h = 16;
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
		}
		else if(!strcmp(argv[i], "-pack_mode"))
		{
			std::string pack_mode_str = argv[++ i];
			if     (pack_mode_str == "gb")  pack_mode = Tile::GB;
			else if(pack_mode_str == "sgb") pack_mode = Tile::SGB;
			else if(pack_mode_str == "sms") pack_mode = Tile::SMS;
			else 
			{
				printf("-pack_mode must be one of gb, sgb or sms\n");
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-tile_origin"))
		{
			tile_origin = atoi(argv[++ i]);
		}
	}

	pal_size = 1 << bpp;

	if(export_as_map)
		tile_h = 8; //Force tiles_h to 8 on maps
	
	int slash_pos = (int)output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)output_filename.find_last_of('\\');
	int dot_pos = (int)output_filename.find_first_of('.', slash_pos == -1 ? 0 : slash_pos);

	string output_filename_h = output_filename.substr(0, dot_pos) + ".h";
	string data_name = output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(data_name.begin(), data_name.end(), '-', '_');

  //load and decode png
	vector<unsigned char> buffer;
	lodepng::load_file(buffer, argv[1]);
	lodepng::State state;
	if(keep_palette_order)
	{
		//Calling with keep_palette_order means
		//-The image is png8
		//-Each 4 colors define a gbc palette, the first color is the transparent one
		//-Each rectangle with dimension(8, tile_h) in the image has colors from one of those palettes only
		state.info_raw.colortype = LCT_PALETTE;
		state.info_raw.bitdepth = 8;
		state.decoder.color_convert = false;
		unsigned error = lodepng::decode(image.data, image.w, image.h, state, buffer);
		if(error)
		{
			printf("decoder error %s\n", lodepng_error_text(error));
			return 1;
		}

		if(state.info_raw.colortype != LCT_PALETTE)
		{
			printf("error: keep_palette_order only works with png8");
			return 1;
		}

		image.palettesize = state.info_raw.palettesize;
		image.palette = state.info_raw.palette;
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
		if( ((image32.w % TILE_W) != 0) || ((image32.h % tile_h) != 0) )
		{
			printf("Error: Image size %d x %d isn't an even multiple of tile size %d x %d\n", image32.w, image32.h, TILE_W, tile_h);
			return 1;
		}

		int* palettes_per_tile = new int[(image32.w / 8) * (image32.h /tile_h)];
		vector< SetPal > palettes;
		for(unsigned int y = 0; y < image32.h; y += tile_h)
		{
			for(unsigned int x = 0; x < image32.w; x += 8)
			{
				//Get palette colors on (x, y, 8, tile_h)
				SetPal pal = GetPaletteColors(image32, x, y, 8, tile_h);
				if(pal.size() > pal_size)
				{
					printf("Error: more than %d colors found in tile at x:%d, y:%d of size w:%d, h:%d\n", (unsigned int)pal_size, x, y, 8, tile_h);
					return 1;
				}

				//Check if it matches any palettes or create a new one
				size_t i;
				for(i = 0; i < palettes.size(); ++i)
				{
					//Try to merge this palette wit any of the palettes (checking if they are equal is not enough since the palettes can have less than 4 colors)
					SetPal merged(palettes[i]);
					merged.insert(pal.begin(), pal.end());
					if(merged.size() <= pal_size)
					{
						if(palettes[i].size() <= pal_size)
							palettes[i] = merged; //Increase colors with this palette (it has less than 4 colors)
						break; //Found palette
					}
				}

				if(i == palettes.size())
				{
					//Palette not found, add a new one
					if(palettes.size() == max_palettes)
					{
						printf("Error: more than %d palettes found\n", (unsigned int)max_palettes);
						return 1;
					}

					palettes.push_back(pal);
				}

				palettes_per_tile[(y / tile_h) * (image32.w / 8) + (x / 8)] = i;
			}
		}
		
		//Create the indexed image
		image.data.clear();
		image.w = image32.w;
		image.h = image32.h;
		
		image.palettesize = palettes.size() * pal_size;
		image.palette = new unsigned char[palettes.size() * pal_size * 4]; //pal_size colors * 4 bytes each
		for(size_t p = 0; p < palettes.size(); ++ p)
		{
			int *color_ptr = (int*)&image.palette[p * pal_size * 4];

			//TODO: if palettes[p].size() != pal_size we should probably try to fill the gaps based on grayscale values 

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
				unsigned char* c32ptr = &image32.data[(image32.w * y + x) * 4];
				int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
				unsigned char palette = palettes_per_tile[(y / tile_h) * (image32.w / 8) + (x / 8)];
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
  
	//Output .h FILE
	FILE* file = fopen(output_filename_h.c_str(), "w");
	if(!file) {
		printf("Error writing file");
		return 1;
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
		fprintf(file, "#define %s_TILE_H %d\n", data_name.c_str(), tile_h);
		fprintf(file, "#define %s_WIDTH %d\n",  data_name.c_str(), sprite_w);
		fprintf(file, "#define %s_HEIGHT %d\n", data_name.c_str(), sprite_h);
		fprintf(file, "#define %s_TILE_COUNT %d\n", data_name.c_str(), (unsigned int)tiles.size() * (tile_h >> 3));
		if(export_as_map)
		{
			fprintf(file, "#define %s_MAP_ATTRIBUTES ",  data_name.c_str());
			if(use_map_attributes && map_attributes.size())
				fprintf(file, "%s_map_attributes\n", data_name.c_str());
			else
				fprintf(file, "0\n");

			fprintf(file, "#define %s_TILE_PALS ",  data_name.c_str());
			if(use_map_attributes)
				fprintf(file, "0\n");
			else
				fprintf(file, "%s_tile_pals\n", data_name.c_str());
		}
		else
		{
			fprintf(file, "#define %s_PIVOT_X %d\n", data_name.c_str(), pivot_x);
			fprintf(file, "#define %s_PIVOT_Y %d\n", data_name.c_str(), pivot_y);
			fprintf(file, "#define %s_PIVOT_W %d\n", data_name.c_str(), pivot_w);
			fprintf(file, "#define %s_PIVOT_H %d\n", data_name.c_str(), pivot_h);
		}
		fprintf(file, "\n");
		fprintf(file, "BANKREF_EXTERN(%s)\n", data_name.c_str());
		fprintf(file, "\n");
		fprintf(file, "extern const palette_color_t %s_palettes[%d];\n", data_name.c_str(), (unsigned int)image.palettesize);
		fprintf(file, "extern const uint8_t %s_tiles[%d];\n", data_name.c_str(), (unsigned int)(tiles.size() * (8 * tile_h * bpp / 8)));
		
		fprintf(file, "\n");
		if(export_as_map)
		{
			fprintf(file, "extern const unsigned char %s_map[%d];\n", data_name.c_str(), (unsigned int)map.size());

			if(use_map_attributes) {
				if(map_attributes.size()) {
					fprintf(file, "extern const unsigned char %s_map_attributes[%d];\n", data_name.c_str(), (unsigned int)map_attributes.size());
				}
			} else
				fprintf(file, "extern const unsigned char* %s_tile_pals[%d];\n", data_name.c_str(), (unsigned int)tiles.size());
		}
		else
		{
			fprintf(file, "extern const metasprite_t* const %s_metasprites[%d];\n", data_name.c_str(), (unsigned int)sprites.size());
		}
	}
	fprintf(file, "\n");
	fprintf(file, "#endif");
	

	fclose(file);

	//Output .c FILE
	file = fopen(output_filename.c_str(), "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}
	
	if (bank) fprintf(file, "#pragma bank %d\n\n", bank);

	fprintf(file, "//AUTOGENERATED FILE FROM png2asset\n\n");

	fprintf(file, "#include <stdint.h>\n");
	fprintf(file, "#include <gbdk/platform.h>\n");
	fprintf(file, "#include <gbdk/metasprites.h>\n");
	fprintf(file, "\n");

	fprintf(file, "BANKREF(%s)\n\n", data_name.c_str());

	fprintf(file, "const palette_color_t %s_palettes[%d] = {\n", data_name.c_str(), (unsigned int)image.palettesize);
	for(size_t i = 0; i < image.palettesize / 4; ++i)
	{
		if(i != 0)
			fprintf(file, ",\n");
		fprintf(file, "\t");

		unsigned char* pal_ptr = &image.palette[i * 16];
		for(int c = 0; c < 4; ++ c, pal_ptr += 4)
		{
			fprintf(file, "RGB8(%d, %d, %d)", pal_ptr[0], pal_ptr[1], pal_ptr[2]);
			if(c != 3)
				fprintf(file, ", ");
		}
	}
	fprintf(file, "\n};\n");

	fprintf(file, "\n");
	fprintf(file, "const uint8_t %s_tiles[%d] = {\n", data_name.c_str(), (unsigned int)(tiles.size() * 8 * tile_h * bpp / 8));
	for(vector< Tile >::iterator it = tiles.begin(); it != tiles.end(); ++ it)
	{
		fprintf(file, "\t");

		vector< unsigned char > packed_data = (*it).GetPackedData(pack_mode);
		for(vector< unsigned char >::iterator it2 = packed_data.begin(); it2 != packed_data.end(); ++ it2)
		{
			fprintf(file, "0x%02x", (*it2));
			if((it + 1) != tiles.end() || (it2 + 1) != packed_data.end())
				fprintf(file, ",");
		}

		if(it != tiles.end())
			fprintf(file, "\n");
	}
	fprintf(file, "};\n\n");

	if(!export_as_map)
	{
		for(vector< MetaSprite >::iterator it = sprites.begin(); it != sprites.end(); ++ it)
		{
			fprintf(file, "const metasprite_t %s_metasprite%d[] = {\n", data_name.c_str(), (int)(it - sprites.begin()));
			fprintf(file, "\t");
			for(MetaSprite::iterator it2 = (*it).begin(); it2 != (*it).end(); ++ it2)
			{
				fprintf(file, "METASPR_ITEM(%d, %d, %d, %d), ", (*it2).offset_y, (*it2).offset_x, (*it2).offset_idx, (*it2).props);
			}
			fprintf(file, "METASPR_TERM\n");
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
			fprintf(file, "\t%d, //num palettes\n", (unsigned int)(image.palettesize / pal_size));
			fprintf(file, "\t%s_palettes, //CGB palette\n", data_name.c_str());
			fprintf(file, "\t%d, //num sprites\n", (unsigned int)sprites.size());
			fprintf(file, "\t%s_metasprites, //metasprites\n", data_name.c_str());
			fprintf(file, "};\n");
		}
	}
	else
	{
		//Export tiles pals (if any)
		if(!use_map_attributes)
		{
			fprintf(file, "\n");
			fprintf(file, "const uint8_t %s_tile_pals[%d] = {\n\t", data_name.c_str(), (unsigned int)tiles.size());
			for(vector< Tile >::iterator it = tiles.begin(); it != tiles.end(); ++ it)
			{
				if(it != tiles.begin())
					fprintf(file, ", ");
				fprintf(file, "%d", it->pal);
			}
			fprintf(file, "\n};\n");
		}

		if(use_structs)
		{
			//Export Tiles Info
			fprintf(file, "\n");
			fprintf(file, "#include \"TilesInfo.h\"\n");
			fprintf(file, "BANKREF(%s_tiles_info)\n", data_name.c_str());
			fprintf(file, "const struct TilesInfo %s_tiles_info = {\n", data_name.c_str());
			fprintf(file, "\t%d, //num tiles\n", (unsigned int)tiles.size() * (tile_h >> 3));
			fprintf(file, "\t%s_tiles, //tiles\n", data_name.c_str());
			fprintf(file, "\t%d, //num palettes\n", (unsigned int)(image.palettesize / pal_size));
			fprintf(file, "\t%s_palettes, //palettes\n", data_name.c_str());
			if(!use_map_attributes)
				fprintf(file, "\t%s_tile_pals, //tile palettes\n", data_name.c_str());
			else
				fprintf(file, "\t0 //tile palettes\n");
			fprintf(file, "};\n");
		}

		//Export map
		fprintf(file, "\n");
		fprintf(file, "const unsigned char %s_map[%d] = {\n", data_name.c_str(), (unsigned int)map.size());
		size_t line_size = map.size() / (image.h / 8);
		for(size_t j = 0; j < image.h / 8; ++ j)
		{
			fprintf(file, "\t");
			for(size_t i = 0; i < line_size; ++ i)
			{
				fprintf(file, "0x%02x,", map[j * line_size + i]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "};\n");

		//Export map attributes (if any)
		if(use_map_attributes && map_attributes.size())
		{
			fprintf(file, "\n");
			fprintf(file, "const unsigned char %s_map_attributes[%d] = {\n", data_name.c_str(), (unsigned int)map_attributes.size());
			for(size_t j = 0; j < image.h / 8; ++ j)
			{
				fprintf(file, "\t");
				for(size_t i = 0; i < line_size; ++ i)
				{
					fprintf(file, "0x%02x,", map_attributes[j * line_size + i]);
				}
				fprintf(file, "\n");
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
			if(use_map_attributes && map_attributes.size())
				fprintf(file, "\t%s_map_attributes, //map attributes\n", data_name.c_str());
			else
				fprintf(file, "\t%s, //map attributes\n", "0");
			fprintf(file, "\tBANK(%s_tiles_info), //tiles bank\n", data_name.c_str());
			fprintf(file, "\t&%s_tiles_info, //tiles info\n", data_name.c_str());
			fprintf(file, "};\n");
		}
	}
	fclose(file);
}
