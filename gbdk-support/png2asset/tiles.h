#pragma once
#include <vector>

using namespace std;

#define BIT(VALUE, INDEX) (1 & ((VALUE) >> (INDEX)))


struct ExtraPalette {

    unsigned char palette;
    vector< unsigned char> pixelData;
};

struct Tile
{
    vector< ExtraPalette> extraPaletteInfo;
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
        NES,
        SGB,
        SMS,
        BPP1
    };

    vector< unsigned char > GetPackedData(PackMode pack_mode, int tile_w, int tile_h, int bpp) {
        vector< unsigned char > ret((tile_w / 8) * tile_h * bpp, 0);
        if(pack_mode == GB) {
            for(int j = 0; j < tile_h; ++j) {
                for(int i = 0; i < 8; ++i) {
                    unsigned char col = data[8 * j + i];
                    ret[j * 2] |= BIT(col, 0) << (7 - i);
                    ret[j * 2 + 1] |= BIT(col, 1) << (7 - i);
                }
            }
        }        
        else if(pack_mode == NES) {
            for(int j = 0; j < tile_h; ++j) {
                for(int i = 0; i < 8; ++i) {
                    unsigned char col = data[8 * j + i];
                    ret[j] |= BIT(col, 0) << (7 - i);
                    ret[j + 8] |= BIT(col, 1) << (7 - i);
                }
            }
        }
        else if(pack_mode == SGB)
        {
            for(int j = 0; j < tile_h; ++j) {
                for(int i = 0; i < 8; ++i) {
                    unsigned char col = data[8 * j + i];
                    ret[j * 2] |= BIT(col, 0) << (7 - i);
                    ret[j * 2 + 1] |= BIT(col, 1) << (7 - i);
                    ret[(tile_h + j) * 2] |= BIT(col, 2) << (7 - i);
                    ret[(tile_h + j) * 2 + 1] |= BIT(col, 3) << (7 - i);
                }
            }
        }
        else if(pack_mode == SMS)
        {
            for(int j = 0; j < tile_h; ++j) {
                for(int i = 0; i < 8; ++i) {
                    unsigned char col = data[8 * j + i];
                    ret[j * 4] |= BIT(col, 0) << (7 - i);
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


Tile FlipH(const Tile& tile);
Tile FlipV(const Tile& tile);