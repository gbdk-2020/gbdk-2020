#include "tiles.h"

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
