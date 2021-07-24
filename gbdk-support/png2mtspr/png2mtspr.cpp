#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>

using namespace std;

int decodePNG(vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
void loadFile(vector<unsigned char>& buffer, const std::string& filename);

#define BIT(VALUE, INDEX) (1 & ((VALUE) >> (INDEX)))

typedef vector< unsigned char > Tile;

struct PNGImage
{
	vector< unsigned char > data;
	unsigned long w;
	unsigned long h;

	unsigned char* GetColor(int x, int y)
	{
		return &data[(w * y + x) * 4];
	}

	unsigned char GetGBColor(int x, int y)
	{
		unsigned char* color = GetColor(x, y);
		if(color[3] == 0) //alpha == 0
			return 0;
		float col = 255 - ((color[0] * 0.3f) + (color[1] * 0.59f) + (color[2] * 0.11f));
		unsigned char ret = ((unsigned char) col) >> 6;
		if(ret < 2)
			return ret < 1 ? 1 : 2;
		else
			return ret;
	}

	bool ExtractGBTile(int x, int y, int tile_h, Tile& tile)
	{
		bool all_zero = true;
		for(int j = 0; j < tile_h; ++ j)
		{
			unsigned char& l = tile[j * 2];
			unsigned char& h = tile[j * 2 + 1];
			l = h = 0;

			for(int i = 0; i < 8; ++i)
			{
				unsigned char color = GetGBColor(x + i, y + j);
				l |= BIT(color, 0) << (7 - i);
				h |= BIT(color, 1) << (7 - i);
			}

			all_zero = all_zero && (l == 0) && (h == 0);
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
PNGImage image;
int tile_h;
int props_default = 0x00;  // Default Sprite props has no attributes enabled

Tile FlipH(const Tile& tile)
{
	Tile ret;
	for(unsigned int i = 0; i < tile.size(); i += 2)
	{
		ret.push_back(tile[tile.size() - 1 - i - 1]);
		ret.push_back(tile[tile.size() - 1 - i]);
	}
	return ret;
}

Tile FlipV(const Tile& tile)
{
	Tile ret;
	for(unsigned int i = 0; i < tile.size(); ++i)
	{
		const unsigned char& c0 = tile[i];
		unsigned char c1 = 0;
		for(int j = 0; j < 8; ++j)
		{
			c1 |= BIT(c0, 7 - j) << j;
		}
		ret.push_back(c1);
	}
	return ret;
}

bool FindTile(const Tile& t, unsigned char& idx, unsigned char& props)
{
	vector< Tile >::iterator it;
	it = find(tiles.begin(), tiles.end(), t);
	if(it != tiles.end())
	{
		idx = (unsigned char)(it - tiles.begin());
		props = props_default;
		return true;
	}
	
	Tile tile = FlipV(t);
	it = find(tiles.begin(), tiles.end(), tile);
	if(it != tiles.end())
	{
		idx = (unsigned char)(it - tiles.begin());
		props = props_default | (1 << 5);
		return true;
	}

	tile = FlipH(tile);
	it = find(tiles.begin(), tiles.end(), tile);
	if(it != tiles.end())
	{
		idx = (unsigned char)(it - tiles.begin());
		props = props_default | (1 << 5) | (1 << 6);
		return true;
	}

	tile = FlipV(tile);
	it = find(tiles.begin(), tiles.end(), tile);
	if(it != tiles.end())
	{
		idx = (unsigned char)(it - tiles.begin());
		props = props_default | (1 << 6);
		return true;
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
			Tile tile(tile_h * 2);
			if (image.ExtractGBTile(x, y, tile_h, tile))
			{
				unsigned char idx;
				unsigned char props;
				if(!FindTile(tile, idx, props))
				{
					tiles.push_back(tile);
					idx = (unsigned char)tiles.size() - 1;
					props = props_default;
				}

				if(tile_h == 16)
					idx *= 2;

				mt_sprite.push_back(MTTile(x - last_x, y - last_y, idx, props));
				
				last_x = x;
				last_y = y;
			}
		}
	}
	// Changed this code so empty metasprites are no longer dropped
	//
	// Metasprite was empty (made entirely of transparent tiles)
	// if(mt_sprite.size() == 0) {
	//	sprites.pop_back();
	// }
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("usage: png2mtspr <file>.png [options]\n");
		printf("-c            ouput file (default: <png file>.c)\n");
		printf("-sw <width>   metasprites width size (default: png width)\n");
		printf("-sh <height>  metasprites height size (default: png height)\n");
		printf("-sp <props>   change default for sprite OAM property bytes (in hex) (default: 0x00)\n");
		printf("-px <x coord> metasprites pivot x coordinate (default: metasprites width / 2)\n");
		printf("-py <y coord> metasprites pivot y coordinate (default: metasprites height / 2)\n");
		printf("-spr8x8       use SPRITES_8x8 (default: SPRITES_8x16)\n");
		printf("-spr8x16      use SPRITES_8x16 (default: SPRITES_8x16)\n");
		printf("-b <bank>     bank (default 0)\n");
		return 0;
	}

	//default params
	int sprite_w = 0;
	int sprite_h = 0;
	int pivot_x = 0xFFFFFF;
	int pivot_y = 0xFFFFFF;
	tile_h = 16;
  string output_filename = argv[1];
	output_filename = output_filename.substr(0, output_filename.size() - 4) + ".c";
	int bank = 0;

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
	}

	int slash_pos = (int)output_filename.find_last_of('/');
	if(slash_pos == -1)
		slash_pos = (int)output_filename.find_last_of('\\');
	int dot_pos = (int)output_filename.find_last_of('.');

	if(slash_pos == -1) slash_pos = -1;
	if(dot_pos == -1) dot_pos = -1;

	string output_filename_h = output_filename.substr(0, dot_pos) + ".h";
	string data_name = output_filename.substr(slash_pos + 1, dot_pos - 1 - slash_pos);
	replace(data_name.begin(), data_name.end(), '-', '_');
	printf("data_name:%s", data_name.c_str());
	printf("\n"); // End of output

  //load and decode png
  vector<unsigned char> buffer;
  loadFile(buffer, argv[1]);
  if(decodePNG(image.data, image.w, image.h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size()) != 0) 
	{
		printf("Error decoding PNG");
		return 0;
	}

	if(sprite_w == 0) sprite_w = (int)image.w;
	if(sprite_h == 0) sprite_h = (int)image.h;
	if(pivot_x == 0xFFFFFF) pivot_x = sprite_w / 2;
	if(pivot_y == 0xFFFFFF) pivot_y = sprite_h / 2;

	//Extract metasprites
	for(int y = 0; y < (int)image.h; y += sprite_h)
	{
		for(int x = 0; x < (int)image.w; x += sprite_w)
		{
			GetMetaSprite(x, y, sprite_w, sprite_h, pivot_x, pivot_y);
		}
	}
  
	//Output .h FILE
	FILE* file = fopen(output_filename_h.c_str(), "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "//AUTOGENERATED FILE FROM png2mtspr\n");
	fprintf(file, "#define %s_TILE_H %d\n", data_name.c_str(), tile_h);
	fprintf(file, "#define %s_WIDTH %d\n",  data_name.c_str(), sprite_w);
	fprintf(file, "#define %s_HEIGHT %d\n", data_name.c_str(), sprite_h);
	fprintf(file, "#define %s_PIVOT_X %d\n", data_name.c_str(), pivot_x);
	fprintf(file, "#define %s_PIVOT_Y %d\n", data_name.c_str(), pivot_y);
	fprintf(file, "#define %s_TILE_COUNT %d\n", data_name.c_str(), (int)tiles.size());
	fprintf(file, "#define %s_METASPRITE_COUNT %d\n", data_name.c_str(), (int)sprites.size());
	fprintf(file, "\n");
	fprintf(file, "extern const UINT8 %s_data[%d];\n", data_name.c_str(), (int)(tiles.size() * tile_h * 2));
	fprintf(file, "extern const metasprite_t* const %s_metasprites[%d];\n", data_name.c_str(), (int)sprites.size());

	fclose(file);

	//Output .c FILE
	file = fopen(output_filename.c_str(), "w");
	if(!file) {
		printf("Error writing file");
		return 1;
	}

	fprintf(file, "//AUTOGENERATED FILE FROM png2mtspr\n\n");
	fprintf(file, "#include <gb/gb.h>\n");
	fprintf(file, "#include <gb/metasprites.h>\n");
	fprintf(file, "\n");

	if(bank)
		fprintf(file, "#pragma bank %d\n\n", bank);

	fprintf(file, "const UINT8 %s_data[%d] = {\n", data_name.c_str(), (int)(tiles.size() * tile_h * 2));
	for(vector< Tile >::iterator it = tiles.begin(); it != tiles.end(); ++ it)
	{
		for(Tile::iterator it2 = (*it).begin(); it2 != (*it).end(); ++ it2)
		{
			fprintf(file, "0x%02x", (*it2));
			if((it + 1) != tiles.end() || (it2 + 1) != (*it).end())
				fprintf(file, ",");
		}

		if(it != tiles.end())
			fprintf(file, "\n");
	}
	fprintf(file, "};\n\n");

	for(vector< MetaSprite >::iterator it = sprites.begin(); it != sprites.end(); ++ it)
	{
		fprintf(file, "const metasprite_t %s_metasprite%d[] = {\n", data_name.c_str(), (int)(it - sprites.begin()));
		fprintf(file, "\t");
		for(MetaSprite::iterator it2 = (*it).begin(); it2 != (*it).end(); ++ it2)
		{
			fprintf(file, "{%d, %d, %d, %d}, ", (*it2).offset_y, (*it2).offset_x, (*it2).offset_idx, (*it2).props);
		}
		fprintf(file, "{metasprite_end}\n");
		fprintf(file, "};\n\n");
	}

	fprintf(file, "const metasprite_t* const %s_metasprites[%d] = {\n\t", data_name.c_str(), (int)sprites.size());
	for(vector< MetaSprite >::iterator it = sprites.begin(); it != sprites.end(); ++ it)
	{
		fprintf(file, "%s_metasprite%d", data_name.c_str(), (int)(it - sprites.begin()));
		if(it + 1 != sprites.end())
			fprintf(file, ", ");
	}
	fprintf(file, "\n};\n");

	fclose(file);
}
