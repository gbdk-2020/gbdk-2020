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
#include "export.h"

#include "cmp_int_color.h"

using namespace std;

#include "png2asset.h"
#include "image_utils.h"
#include "palettes.h"

bool GetSourceTileset(bool repair_indexed_pal, bool keep_palette_order, unsigned int max_palettes, vector< SetPal >& palettes) {

	lodepng::State sourceTilesetState;
	vector<unsigned char> buffer2;

	lodepng::load_file(buffer2, source_tileset);

	// TODO: This code block below is mostly identical (aside from memcpy, var names) to the main indexed image load.
	//       It should get deduplicated into a function.
	if(keep_palette_order) {
		//Calling with keep_palette_order means
		//-The image should be png indexed (1-8 bits per pixel)
		//-For CGB: Each 4 colors define a gbc palette, the first color is the transparent one
		//-Each rectangle with dimension(tile_w, tile_h) in the image has colors from one of those palettes only
		sourceTilesetState.info_raw.colortype = LCT_PALETTE;
		sourceTilesetState.info_raw.bitdepth = 8;

		// * Do *NOT* turn color_convert ON here.
		// When source PNG is indexed with bit depth was less than 8 bits per pixel then
		// color_convert may mangle the packed indexed values. Instead manually unpack them.
		//
		// This is necessary since some PNG encoders will use the minimum possible number of bits.
		//   For example 2 colors in the palette -> 1bpp -> 8 pixels per byte in the decoded image.
		//     Also see below about requirement to use palette from source image
		sourceTilesetState.decoder.color_convert = false;

		unsigned error = lodepng::decode(source_tileset_image.data, source_tileset_image.w, source_tileset_image.h, sourceTilesetState, buffer2);
		// Unpack the image if needed. Also checks and errors on incompatible palette type if needed
		if(!image_indexed_ensure_8bpp(source_tileset_image.data, source_tileset_image.w, source_tileset_image.h, (int)sourceTilesetState.info_png.color.bitdepth, (int)sourceTilesetState.info_png.color.colortype))
			return false;
		else if(error) {
			printf("decoder error %s\n", lodepng_error_text(error));
			return false;
		}

		// Use source image palette since lodepng conversion to indexed (LCT_PALETTE) won't create a palette
		// So: source_tileset_image.info_png.color.palette/size instead of source_tileset_image.info_raw.palette/size
		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, sourceTilesetState.info_png.color.palettesize / source_tileset_image.colors_per_pal);
		source_tileset_image.total_color_count = palette_count * source_tileset_image.colors_per_pal;
		source_tileset_image.palette = (uint8_t*)malloc(source_tileset_image.total_color_count * RGBA32_SZ);
		if(source_tileset_image.palette) {
			memcpy(source_tileset_image.palette, sourceTilesetState.info_png.color.palette, source_tileset_image.total_color_count * RGBA32_SZ);
		}

		if(repair_indexed_pal)
			if(!image_indexed_repair_tile_palettes(source_tileset_image, use_2x2_map_attributes))
				return 1;
	}
	else {

		PNGImage image32;
		unsigned error = lodepng::decode(image32.data, image32.w, image32.h, sourceTilesetState, buffer2); //decode as 32 bit
		if(error)
		{
			printf("decoder error %s\n", lodepng_error_text(error));
			return false;
		}

		image32.colors_per_pal = source_tileset_image.colors_per_pal;
		image32.tile_w = source_tileset_image.tile_w;
		image32.tile_h = source_tileset_image.tile_h;
		int* palettes_per_tile = BuildPalettesAndAttributes(image32, palettes, use_2x2_map_attributes);

		//Create the indexed image
		source_tileset_image.data.clear();
		source_tileset_image.w = image32.w;
		source_tileset_image.h = image32.h;

		unsigned int palette_count = PaletteCountApplyMaxLimit(max_palettes, palettes.size());

		source_tileset_image.total_color_count = palette_count * source_tileset_image.colors_per_pal;
		source_tileset_image.palette = new unsigned char[palette_count * source_tileset_image.colors_per_pal * RGBA32_SZ]; //colors_per_pal colors * 4 bytes each
		source_total_color_count = source_tileset_image.total_color_count;

		for(size_t p = 0; p < palette_count; ++p)
		{
			int* color_ptr = (int*)&source_tileset_image.palette[p * source_tileset_image.colors_per_pal * RGBA32_SZ];

			//TODO: if palettes[p].size() != colors_per_pal we should probably try to fill the gaps based on grayscale values

			for(SetPal::iterator it = palettes[p].begin(); it != palettes[p].end(); ++it, color_ptr++)
			{
				unsigned char* c = (unsigned char*)&(*it);
				*color_ptr = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
			}
		}

		for(size_t y = 0; y < image32.h; ++y)
		{
			for(size_t x = 0; x < image32.w; ++x)
			{
				unsigned char* c32ptr = &image32.data[(image32.w * y + x) * RGBA32_SZ];
				int color32 = (c32ptr[0] << 24) | (c32ptr[1] << 16) | (c32ptr[2] << 8) | c32ptr[3];
				unsigned char palette = palettes_per_tile[(y / image32.tile_h) * (image32.w / image32.tile_w) + (x / image32.tile_w)];
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
	printf("Got %d palettes from the source tileset.\n", (unsigned int)(source_tileset_image.total_color_count / source_tileset_image.colors_per_pal));

	return true;

}
