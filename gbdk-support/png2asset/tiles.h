#pragma once
#include <vector>

using namespace std;

#define BIT(VALUE, INDEX) (1 & ((VALUE) >> (INDEX)))

struct Tile
{
    vector< unsigned char > data;
    unsigned char pal;

    Tile(size_t size = 0) : data(size), pal(0) {}
    bool operator==(const Tile& t) const
    {
//        return data == t.data && pal == t.pal; // probably, sometimes we need to take palette into account?
        return data == t.data;
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
        GG,
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
                // For NES pack mode, tiles are planar instead of interleaved.
                // 8 bytes for bit0 rows, then 8 bytes for bit1 rows.
                // This means 8x16 tiles still need to be treated as two separate
                // 8x8 tiles, in contrast to GB pack mode.
                int tilenum = j / 8;
                int tilerow = j % 8;
                for(int i = 0; i < 8; ++i) {
                    unsigned char col = data[8 * j + i];
                    ret[16 * tilenum + tilerow] |= BIT(col, 0) << (7 - i);
                    ret[16 * tilenum + tilerow + 8] |= BIT(col, 1) << (7 - i);
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
        else if ((pack_mode == SMS) || (pack_mode == GG))
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