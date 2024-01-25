
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "hicolour.h"
#include "median.h"
#include "wu.h"


#include <common.h>
#include <options.h>
#include <files.h>
#include <image_info.h>
#include <logging.h>
#include <c_source.h>
#include <tile_dedupe.h>

/* Gameboy Hi-Colour Convertor */
/* Glen Cook */
/* Jeff Frohwein */
/* Rob Jones */


/*

This code is based on the code written by Jeff Frohwein. Jeff originally wrote a 128x128
Gameboy HiColour convertor and made the source publically available. The problem with
the original work, is that the output from the original code had a large white border
making the picture look framed.

The original code was then modified by another party to produce a full screen image, using
a fixed attribute block size of 3-2-3-2-3-2-3-2. The output from this modified code looked
great, but some pictures had artifacts, due to the fixed attribute size.

I then decided to modify the full screen code, to produce pictures with less artifacts, the
attribute blocks are not fixed, they can adapt their size based on the type of picture that
is being converted.

This program will step through every possible combination of attributes to find the best
possible solution.

The program gives the user the option of using fixed or adaptive attribute blocks, fixed
attribute blocks are much quicker to calculate, but the picture quality may not be perfect.

After creating a DOS version of this program, I then went ahead and wrote a windows interface
for it, to tidy it up, and also give me the chance to learn some windows programming. This is
my first windows program, so please be kind.

The best method for converting the pictures, is to use Adaptive method 3, although this can
take quite a bit longer to calculate than the fixed size calculations.

I believe that the new median cut method with dither produces the best results in general,
but the other quantisers can produce better results for other picture types.

I am releasing this program into the public domain, feel free to adapt it in anyway that you
deem fit. I you feel you have improved this program in anyway, drop me a line, and I will
incorperate the changes into newer versions. (GlenCook@hotmail.com)

*/


/* HISTORY */


// V1.0 - 27th March 2000 - First public release
// V1.1 - 30th March 2000 - Rob Jones added seperate thread for conversion process
// V1.2 - 8th  April 2000 - Added other quantisation methods
// V1.4 -            2023 - Converted to cross platform console utility with PNG support (bbbbbr)


// Function prototypes

int    br,bg,bb;

typedef struct
{
    u8        p1;
    u8        p2;
    u8        FileType;
    u8        p3[9];
    u16       XSize;
    u16       YSize;
    u8        BitDepth;
    u8        c1;
    u8        data[160*BUF_HEIGHT][3];
} IMG_TYPE;


// u8            QR[BUF_HEIGHT][160][3];
u8            TileOffset[4];                 // Offset into screen for attribute start
u8            TileWidth[4];                  // No of character attributes width
u8            Pal[8][BUF_Y_REGION_COUNT_LR_RNDUP][28][3];             // Palettes for every other line
u8            IdealPal[8][BUF_Y_REGION_COUNT_LR_RNDUP][4][3];         // The best fit palette
u8            pic[160][BUF_HEIGHT][3];              // Original Picture
u8            pic2[160][BUF_HEIGHT][3];             // Output picture
u8            out[160][BUF_HEIGHT];                 // Output data

u8            raw[2][160][BUF_HEIGHT][3];           // Original Picture Raw format.
                                             // sourced from [0] = Normal , [1] = GB Color selected by ViewType

// TODO: delete?
s32           ViewType=0;                    // Type of view to show: 0 = Normal , 1 = GB Color

u8            Best[2][BUF_HEIGHT_IN_TILES_RNDUP];  // Best Attribute type to use
u8            LConversion;                   // Conversion type for left hand side of the screen
u8            RConversion;                   // Conversion type for right hand side of the screen
// HWND          Ghdwnd;                          // Global window handle
u8            MapTileIDs[20][BUF_HEIGHT_IN_TILES];           // Attribute table for final render
u8            MapAttributes[20][BUF_HEIGHT_IN_TILES];        // Attribute table for final render
uint8_t       TileSet[20 * BUF_HEIGHT_IN_TILES * TILE_SZ];   // Sequential Tileset Data in Game Boy 2bpp format
uint8_t       TileSetDeduped[20 * BUF_HEIGHT_IN_TILES * TILE_SZ];   // Sequential Tileset Data in Game Boy 2bpp format
unsigned int  TileCountDeduped;
// u8            OldLConv=0;                    // Conversion type
// u8            OldRConv=0;
uint8_t *     pBuffer;
// u8            Message[2000];
s32           ConvertType; //=2;

u8            Data[160*BUF_HEIGHT*3];  // Gets used for quantizing regions. Maybe other things too?

u32           TempD;
s32           BestLine=0;  // TODO: convert to local var
u32           BestQuantLine;
// RGBQUAD       GBView; // converted to local vars



// Shim buffers for the former windows rendered images that were also used for some calculationss
// bmihsource.biWidth          = 160;
// bmihsource.biHeight         = BUF_HEIGHT;
// bmihsource.biPlanes         = 1;
// bmihsource.biBitCount       = 24;
static      uint8_t Bitsdest[160 * BUF_HEIGHT * 3]; // TODO: RGBA 4 bytes per pixel?
static      uint8_t Bitssource[160 * BUF_HEIGHT * 3];
//
static      uint8_t *pBitsdest = Bitsdest;
            uint8_t *pBitssource = Bitssource;


#define MAX_CONVERSION_TYPES    83
#define MAX_QUANTISER_TYPES     4

int image_y_min;
int image_y_max;
int image_height;
int y_region_count_left;
int y_region_count_right;
int y_region_count_lr_rndup;
int y_region_count_both_sides;
int y_height_in_tiles_left;
int y_height_in_tiles_right;
int y_height_in_tiles;
int y_height_in_tiles_lr_rndup;


static void PrepareTileSet(void);
static void PrepareMap(void);
static void PrepareAttributes(void);

static void DedupeTileset(void);

static void ExportPalettes(const char * fname_base);
static void ExportTileSet(const char * fname_base);
static void ExportMap(const char * fname_base);
static void ExportMapAttributes(const char * fname_base);


void hicolor_init(void) {
    // Defaults
    LConversion = 3; // Default Conversion (Fixed 3-2-3-2) Left Screen
    RConversion = 3; // Default Conversion (Fixed 3-2-3-2) Righ Screen
    ConvertType = 1; // Normal default is 1 ("Median cut - no dither")
}

static void hicolor_vars_prep(image_data * p_loaded_image) {
    log_debug("hicolor_vars_prep()\n");

    image_height                = p_loaded_image->height;
    image_y_min                 = 0;
    image_y_max                 = p_loaded_image->height - 1;

    // // Screen palette region updates are 80 pixels wide and 2 pixels tall
    // // since palette 0-3 allocated to left side, 4-7 allocated to right side
    // // and only 4 palettes are updated per scanline, so Left and Right alternate in gettig udpates
    // // 73(L) & 72(R) for standard GB screen

    // One extra region due to starting at -1 Y offset from screen grid, and so there is a last extra entry that "hangs off" the bottom of the screen
    y_region_count_left         = ((image_height / PAL_REGION_HEIGHT_PX) + 1);
    y_region_count_right        =  (image_height / PAL_REGION_HEIGHT_PX);
    // Use larger size[side] for rounded up amount
    y_region_count_lr_rndup     =  (y_region_count_left);
    y_region_count_both_sides   =  (y_region_count_left + y_region_count_right);

    // 19(L) & 18(R) for standard GB Full screen height
    // One extra region due to starting at -1 Y offset from screen grid, and so there is a last extra entry that "hangs off" the bottom of the screen
    y_height_in_tiles_left      = ((image_height / TILE_HEIGHT_PX) + 1);
    y_height_in_tiles_right     =  (image_height / TILE_HEIGHT_PX);
    y_height_in_tiles           =  (image_height / TILE_HEIGHT_PX);
    // Use larger size[side] for rounded up amount
    y_height_in_tiles_lr_rndup  =  (y_height_in_tiles_left);
}



void hicolor_set_convert_left_pattern(uint8_t new_value) {
    // IDC_CONVERTLEFT
    LConversion = new_value;
    log_verbose("HiColor: Left pattern set to %d\n", new_value);
}


void hicolor_set_convert_right_pattern(uint8_t new_value) {
    // IDC_CONVERTRIGHT
    RConversion = new_value;
    log_verbose("HiColor: Right pattern set to %d\n", new_value);
}


void hicolor_set_type(uint8_t new_value) {
    // IDC_CONVERTTYPE
    ConvertType = new_value;
    log_verbose("HiColor: Convert type set to %d\n", new_value);
}


///////////////////////////////////

// Equivalent of former file loading
static void hicolor_image_import(image_data * p_loaded_image) {
    log_debug("hicolor_image_import()\n");

    // TODO: input guarding
    // TODO: deduplicate some of the array copying around
    uint8_t * p_input_img = p_loaded_image->p_img_data;

    for (int y=0; y< image_height; y++) {
        for (int x=0; x< 160; x++) {

            // Clamp to CGB max R/G/B value in RGB 888 mode (31u << 3)
            // png_image[].rgb -> pic2[].rgb -> pBitssource[].bgr??
            pic2[x][y][0] = (p_input_img[RGB_RED]   & 0xf8u);
            pic2[x][y][1] = (p_input_img[RGB_GREEN] & 0xf8u);
            pic2[x][y][2] = (p_input_img[RGB_BLUE]  & 0xf8u);

            p_input_img += RGB_24SZ; // Move to next pixel of source image
        }
    }

    // TODO: Eventually clean up data pathway to remove some vestigial former display rendering buffers
    // It's convoluted, but pBitssource & pBitsdest are used for:
    // - display as windows DIBs (formerly)
    // - and for some calculations at the end of ConvertRegions()
    for (int y=0; y<image_height; y++) {
        for (int x=0; x<160; x++) {
            for (int z=0; z<3; z++) {
                // TODO: (2-z) seems to be swapping RGB for BGR?
                *(pBitssource+(image_y_max-y)*3*160+x*3+z) = pic2[x][y][2-z];            // Invert the dib, cos windows likes it like that !!
            }
        }
    }

}


// TODO: fix
// TODO: Operates on RGB data in pic[] copied from RGB data in pic2
static void hicolor_convert(void) {
    log_debug("hicolor_convert()\n");

    for(int x=0; x<160; x++)
    {
        for(int y=0; y<image_height; y++)
        {
            pic[x][y][0] = pic2[x][y][0];
            pic[x][y][1] = pic2[x][y][1];
            pic[x][y][2] = pic2[x][y][2];

            for(int i=0; i<3; i++)
            {
                *(Data + y*160*3+x*3+i) = pic[x][y][i];
            }
        }
    }

    ConvertToHiColor(ConvertType-1);
}


static void hicolor_save(const char * fname_base) {

    // Default tile count to non-deduplicated number
    int tile_count = y_height_in_tiles * (160 / TILE_WIDTH_PX);

    log_debug("hicolor_save()\n");
    PrepareTileSet();
    PrepareMap();
    PrepareAttributes();

    if (opt_get_tile_dedupe()) {
        DedupeTileset();
        tile_count = TileCountDeduped;
    }

    ExportTileSet(fname_base);
    ExportPalettes(fname_base);
    ExportMap(fname_base);
    ExportMapAttributes(fname_base);

    if (opt_get_c_file_output()) {
        file_c_output_write(fname_base, opt_get_bank_num(), tile_count, y_height_in_tiles);
    }
}


// Currently expects width x height x 3(RGB888)
void hicolor_process_image(image_data * p_loaded_image, const char * fname_base) {
    log_debug("hicolor_process_image(), fname_base: \"%s\"\n", fname_base);

    hicolor_vars_prep(p_loaded_image);
    hicolor_image_import(p_loaded_image);
    hicolor_convert();
    hicolor_save(fname_base);
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static void DedupeTileset(void)
{
    unsigned int map_tile_id;
    uint8_t new_tile_id, new_attribs;

    TileCountDeduped = 0;
    // Traverse all tiles in the image/map
    for (int mapy = 0; mapy < y_height_in_tiles; mapy++) {
        for (int mapx = 0; mapx < 20; mapx++) {

            map_tile_id = MapTileIDs[mapx][mapy];
            map_tile_id += (MapAttributes[mapx][mapy] & CGB_ATTR_TILES_BANK) ? CGB_TILES_START_BANK_1 : CGB_TILES_START_BANK_0;

            if (!tileset_find_matching_tile(&TileSet[map_tile_id * TILE_SZ], &TileSetDeduped[0], TileCountDeduped, &new_tile_id, &new_attribs)) {
                // If no match, copy tile to new tile set and save new index for remapping
                new_tile_id = TileCountDeduped;
                new_attribs = (TileCountDeduped < CGB_TILES_START_BANK_1) ? CGB_ATTR_TILES_BANK_0 : CGB_ATTR_TILES_BANK_1;
                memcpy(&TileSetDeduped[TileCountDeduped * TILE_SZ], &TileSet[map_tile_id * TILE_SZ], TILE_SZ);
                TileCountDeduped++;
            }

            // Update map data and attributes to new index
            // Mask out everything except palettes and then apply the new attribs (bank, vflip, hflip)
            MapTileIDs[mapx][mapy]    = new_tile_id;
            MapAttributes[mapx][mapy] = (MapAttributes[mapx][mapy] & CGB_ATTR_PALETTES_ONLY) | new_attribs;
        }
    }
    log_verbose("DedupeTileset(): Reduced tiles from %d (%d bytes) to %d (%d bytes) = %d bytes saved. %%%d of original size\n",
                map_tile_id + 1, (map_tile_id + 1) * TILE_SZ, TileCountDeduped, TileCountDeduped * TILE_SZ,
                ((map_tile_id + 1) * TILE_SZ) - (TileCountDeduped * TILE_SZ), (TileCountDeduped * 100) / (map_tile_id + 1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void PrepareTileSet(void) {
    uint32_t    byteWritten;
    u32      x, y;
    u8       c1,c2;
    u8       dx,dy;
    u8       c;

    uint8_t * p_buf = TileSet;

    // Write out tilemap data, Left -> Right, Top -> Bottom, 16 bytes per tile
    for (y=0; y<image_height; y=y+8)
    {
        for (x=0; x<160; x=x+8)
        {
            for (dy=0; dy<8; dy++)
            {
                c1 = 0;
                c2 = 0;
                for (dx=0; dx<8; dx++)
                {
                    c1 = (u8)(c1 << 1);
                    c2 = (u8)(c2 << 1);
                    c = out[x+dx][y+dy];
                    if (c & 2) c1++;
                    if (c & 1) c2++;
                }

                *p_buf++ = c2;
                *p_buf++ = c1;
            }
        }
    }
}


static void PrepareMap(void) {
    // Set up export Map Tile IDs
    // Note: The indexes are clipped to 0-255 (instead of 0-512),
    // the attribute tile index+256 bit is auto-calculated in the attribute map in PrepareAttributes()
    int tile_id = 0;
    for (int mapy = 0; mapy < y_height_in_tiles; mapy++) {
        for (int mapx = 0; mapx < 20; mapx++) {

            MapTileIDs[mapx][mapy] = (uint8_t)tile_id;
            tile_id++;
        }
    }
}


static void PrepareAttributes(void) {
    // Set up the Map Attributes table
    unsigned int tile_id = 0;
    for(int MastY=0;MastY<y_height_in_tiles_right;MastY++)
    {
        for(int MastX=0;MastX<2;MastX++)
        {
            int Line=Best[MastX][MastY];
            int width=0;
            for(int i=0;i<4;i++)
            {
                TileOffset[i]=width;
                TileWidth[i]=SplitData[Line][i];
                width+=TileWidth[i];
            }

            for(int x=0;x<4;x++) {
                for(int z=TileOffset[x];z<(TileOffset[x]+TileWidth[x]);z++) {
                    MapAttributes[MastX*10+z][MastY]=x+MastX*4;
                    // Mask in second CGB Tile Bank flag if tile index is over 256 tiles
                    if (tile_id++ >= 256)
                        MapAttributes[MastX*10+z][MastY] |= CGB_ATTR_TILES_BANK_1;
                }
            }
        }
    }
}


static void ExportTileSet(const char * fname_base)
{
    char filename[MAX_PATH*2];

    strcpy(filename, fname_base);
    strcat(filename, ".til");
    log_verbose("Writing Tile Patterns to: %s\n", filename);

    if (opt_get_tile_dedupe()) {

        int outbuf_sz_tiles = TileCountDeduped * TILE_SZ;
        if (!file_write_from_buffer(filename, TileSetDeduped, outbuf_sz_tiles))
            set_exit_error();
    } else {

        int outbuf_sz_tiles = ((image_height / TILE_HEIGHT_PX) * (160 / TILE_WIDTH_PX) * 8 * 2);
        if (!file_write_from_buffer(filename, TileSet, outbuf_sz_tiles))
            set_exit_error();
    }
}


static void ExportPalettes(const char * fname_base)
{
    char filename[MAX_PATH * 2];
    uint32_t    byteWritten;
    uint8_t     tmpByte;
    s32      i, j, k;
    s32      r,g,b,v;

    strcpy(filename, fname_base);
    strcat(filename, ".pal");
    log_verbose("Writing Palette to: %s\n", filename);

    int outbuf_sz_pals = (((y_region_count_both_sides) * 4 * 4 * 2) + 1);
    uint8_t output_buf[outbuf_sz_pals];
    uint8_t * p_buf = output_buf;


    for (i = 0; i < (y_region_count_both_sides); i++) // Number of palette sets (left side updates + right side updates)
    {
        for (j = 0; j < 4; j++) // Each palette in the set
        {
            for(k=0; k<4;k++) // Each color in the palette
            {
                r = IdealPal[(i%2)*4+j][i/2][k][0];
                g = IdealPal[(i%2)*4+j][i/2][k][1];
                b = IdealPal[(i%2)*4+j][i/2][k][2];

                // TODO: Converting to BGR555 probably
                v = ((b/8)*32*32) + ((g/8)*32) + (r/8);

                // 2 bytes per color
                *p_buf++ = (u8)(v & 255);
                *p_buf++ = (u8)(v / 256);
            }
        }
    }

    // TODO: What is this and why? :)
    *p_buf++ = 0x2d;

    if (!file_write_from_buffer(filename, output_buf, outbuf_sz_pals))
        set_exit_error();

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void ExportMap(const char * fname_base)
{
    char      filename[MAX_PATH*2];

    strcpy(filename, fname_base);
    strcat(filename, ".map");
    log_verbose("Writing Tile Map to: %s\n", filename);

    int outbuf_sz_map = (20 * y_height_in_tiles);
    uint8_t output_buf_map[outbuf_sz_map];

    int tile_id = 0;
    for (int y = 0; y < y_height_in_tiles; y++) {
        for (int x = 0; x < 20; x++) {
            uint8_t tile_num = MapTileIDs[x][y];

            // This needs to happen here, after optional deduplication stage
            // since that may rewrite the tile pattern order and indexes
            if (opt_get_map_tile_order() != OPT_MAP_TILE_SEQUENTIAL_ORDER) // implied: OPT_MAP_TILE_ORDER_BY_VRAM_ID
                tile_num = ((tile_num < 128) ? (tile_num) + 128 : (tile_num) - 128); // Previous ordering that was: 128 -> 255 -> 0 -> 127

            output_buf_map[tile_id] = tile_num;
            tile_id++;
        }
    }

    if (!file_write_from_buffer(filename, output_buf_map, outbuf_sz_map))
        set_exit_error();
}


static void ExportMapAttributes(const char * fname_base)
{
    char    filename[MAX_PATH*2];

    strcpy(filename, fname_base);
    strcat(filename, ".atr");
    log_verbose("Writing Attribute Map to: %s\n", filename);

    int outbuf_sz_map = (20 * y_height_in_tiles);
    uint8_t output_buf_map[outbuf_sz_map];

    int tile_id = 0;
    for (int y = 0; y < y_height_in_tiles; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            output_buf_map[tile_id++] = MapAttributes[x][y];
        }
    }

    if (!file_write_from_buffer(filename, output_buf_map, outbuf_sz_map))
        set_exit_error();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// This section of code is used to convert an RGB (pc) triplet into a RGB (gameboy)
// triplet. This section of code was kindly donated by Brett Bibby (GameBrains).

uint8_t intensity[32] =
{
 0x00,0x10,0x20,0x30,0x40,0x50,0x5e,0x6c,0x7a,0x88,0x94,0xa0,0xae,0xb7,0xbf,0xc6,
 0xce,0xd3,0xd9,0xdf,0xe3,0xe7,0xeb,0xef,0xf3,0xf6,0xf9,0xfb,0xfd,0xfe,0xff,0xff
};

unsigned char influence[3][3] =
{
    {16,4,4},
    {8,16,8},
    {0,8,16}
};

RGBQUAD translate(uint8_t rgb[3])
{
    RGBQUAD color;
    uint8_t    tmp[3];
    uint8_t    m[3][3];
    uint8_t    i,j;

    for (i=0;i<3;i++)
        for (j=0;j<3;j++)
            m[i][j] = (intensity[rgb[i]>>3]*influence[i][j]) >> 5;

    for (i=0;i<3;i++)
    {
        if (m[0][i]>m[1][i])
        {
            j=m[0][i];
            m[0][i]=m[1][i];
            m[1][i]=j;
        }

        if (m[1][i]>m[2][i])
        {
            j=m[1][i];
            m[1][i]=m[2][i];
            m[2][i]=j;
        }

        if (m[0][i]>m[1][i])
        {
            j=m[0][i];
            m[0][i]=m[1][i];
            m[1][i]=j;
        }

        tmp[i]=(((m[0][i]+m[1][i]*2+m[2][i]*4)*5) >> 4)+32;
    }

    color.rgbRed    = tmp[0];
    color.rgbGreen    = tmp[1];
    color.rgbBlue    = tmp[2];

    return color;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Data table containing all of the possible combinations of attribute blocks
// for one side of the screen.

// The higher the adaptive level, the more combinations of attributes are tested.

u8    SplitData[80][4]=
{
    {3,2,3,2},{2,3,2,3},{2,2,3,3},{2,3,3,2},{3,2,2,3},{3,3,2,2},{4,2,2,2},{2,2,2,4},{2,2,4,2},{2,4,2,2},{1,1,2,6},
    {1,1,3,5},{1,1,4,4},{1,1,5,3},{1,1,6,2},{1,2,1,6},{1,2,2,5},{1,2,3,4},{1,2,4,3},{1,2,5,2},{1,2,6,1},{1,3,1,5},
    {1,3,2,4},{1,3,3,3},{1,3,4,2},{1,3,5,1},{1,4,1,4},{1,4,2,3},{1,4,3,2},{1,4,4,1},{1,5,1,3},{1,5,2,2},{1,5,3,1},
    {1,6,1,2},{1,6,2,1},{2,1,1,6},{2,1,2,5},{2,1,3,4},{2,1,4,3},{2,1,5,2},{2,1,6,1},{2,2,1,5},{2,2,5,1},{2,3,1,4},
    {2,3,4,1},{2,4,1,3},{2,4,3,1},{2,5,1,2},{2,5,2,1},{2,6,1,1},{3,1,1,5},{3,1,2,4},{3,1,3,3},{3,1,4,2},{3,1,5,1},
    {3,2,1,4},{3,2,4,1},{3,3,1,3},{3,3,3,1},{3,4,1,2},{3,4,2,1},{3,5,1,1},{4,1,1,4},{4,1,2,3},{4,1,3,2},{4,1,4,1},
    {4,2,1,3},{4,2,3,1},{4,3,1,2},{4,3,2,1},{4,4,1,1},{5,1,1,3},{5,1,2,2},{5,1,3,1},{5,2,1,2},{5,2,2,1},{5,3,1,1},
    {6,1,1,2},{6,1,2,1},{6,2,1,1}
};



unsigned int ImageRating(u8 *src, u8 *dest, int StartX, int StartY, int Width, int Height)
{
    log_debug("ImageRating()\n");
    unsigned int    tot;
    int                x,y;
    unsigned int    accum=0;
    int                scradd;

    for(y=StartY;y<(StartY+Height);y++)
    {
        for(x=StartX;x<(StartX+Width);x++)
        {
            scradd=(image_y_max-y)*(160*3)+x*3;
            tot=(*(src+scradd)-*(dest+scradd)) * (*(src+scradd)-*(dest+scradd));
            tot+=(*(src+scradd+1)-*(dest+scradd+1)) * (*(src+scradd+1)-*(dest+scradd+1));
            tot+=(*(src+scradd+2)-*(dest+scradd+2)) * (*(src+scradd+2)-*(dest+scradd+2));
            accum+=tot;
        }
    }

    return accum;
}


// TODO: rename to something that aligns with other convert functions
void ConvertToHiColor(int ConvertType)
{
    log_debug("ConvertToHiColor()\n");
    int        res;
    int        x,y,z,i;
    int        StartSplit=0;
    int        NumSplit=1;
    int        Steps;
    int        MastX,MastY;
    int        Line;
    int        width;
    unsigned int tile_id;

    switch(LConversion)
    {
        case 0:

            StartSplit=0;
            NumSplit=6;
            Steps=504;
            break;

        case 1:

            StartSplit=0;
            NumSplit=10;
            Steps=792;
            break;

        case 2:

            StartSplit=0;
            NumSplit=80;
            Steps=5832;
            break;

        default:

            StartSplit=LConversion-3;
            NumSplit=1;
            Steps=image_height;
            break;
    }

    switch(RConversion)
    {
        case 0:

            Steps+=504;
            break;

        case 1:

            Steps+=792;
            break;

        case 2:

            Steps+=5832;
            break;

        default:

            Steps+=image_height;
            break;
    }


    // Convert left side with one extra tile of height to fix
    // the glitching where the last scanline on left bottom region
    // lacks tile and palette data
    res=ConvertRegions(0,1,0,y_height_in_tiles_left,StartSplit,NumSplit,ConvertType);        // Step through all options
    ConvertRegions(0,1,0,y_height_in_tiles_left,res,1,ConvertType);

    // Formerly: for(y=0;y<189;y++)
    // Treating it as a typo (intended a "18") since 189 would be out of bounds for the original array
    for(y=0;y<y_height_in_tiles_left;y++)
        Best[0][y]=res;


    switch(RConversion)
    {
        case 0:

            StartSplit=0;
            NumSplit=6;
            break;

        case 1:

            StartSplit=0;
            NumSplit=10;
            break;

        case 2:

            StartSplit=0;
            NumSplit=80;
            break;

        default:

            StartSplit=RConversion-3;
            NumSplit=1;
            break;
    }

    for(y=0;y<y_height_in_tiles_right;y++)
    {
        res=ConvertRegions(1,1,y,1,StartSplit,NumSplit,ConvertType);        // Step through all options
        ConvertRegions(1,1,y,1,res,1,ConvertType);
        Best[1][y]=res;
    }


    // TODO: fix me -> pBitsdest being used in conversion process
    for(y=0;y<image_height;y++)
    {
        for(x=0;x<160;x++)
        {
            raw[0][x][y][0] = *(pBitsdest+(image_y_max-y)*3*160+x*3+2);
            raw[0][x][y][1] = *(pBitsdest+(image_y_max-y)*3*160+x*3+1);
            raw[0][x][y][2] = *(pBitsdest+(image_y_max-y)*3*160+x*3);

            RGBQUAD GBView=translate(raw[0][x][y]);

            raw[1][x][y][0] = GBView.rgbRed;
            raw[1][x][y][1] = GBView.rgbGreen;
            raw[1][x][y][2] = GBView.rgbBlue;
        }
    }
    log_progress("\n");
}



// Start X = 0 for Left / 1 for Right
// Width = 1 for half screen 2 = full screen
// StartY = 0 - 17 : Starting attribute block
// Height = Number of attribute blocks to check / process

int ConvertRegions(int StartX, int Width, int StartY, int Height, int StartJ, int FinishJ, int ConvertType)
{
    log_debug("ConvertRegions()\n");
    u32        Accum,width,x1,ts,tw,y2,x2,y_offset;
    s32        x,y;
    s32        i,j;
    u8        col;


    BestQuantLine=0xffffffff;

    for(x=StartX;x<(StartX+Width);x++)
    {
        // Left side of screen is offset by -1 Y
        // (Left side calcs hang off top and bottom of screen
        // due to Left/Right palette update interleaving)
        if (x == CONV_SIDE_LEFT)
            y_offset = CONV_Y_SHIFT_UP_1;
        else
            y_offset = CONV_Y_SHIFT_NO;

        for(j=StartJ;j<(StartJ+FinishJ);j++)
        {
            Accum=0;
            width=0;
            for(i=0;i<4;i++)
            {
                TileOffset[i]=width;
                TileWidth[i]=SplitData[j][i]<<3;
                width+=TileWidth[i];
            }

            for(y=StartY*4;y<(StartY+Height)*4;y++)
            {
                log_progress(".");

                for(x1=0;x1<4;x1++)
                {
                    ts=TileOffset[x1];
                    tw=TileWidth[x1];

                    for(y2=0;y2<2;y2++)
                    {
                        // Skip if Y line is outside image borders (prevents buffer overflow)
                        // (Left side calcs hang off top and bottom of screen
                        // due to Left/Right palette update interleaving)
                        s32 y_line = (y*2+y2-y_offset);
                        if ((y_line < image_y_min) || (y_line > image_y_max)) continue;

                        for(x2=0;x2<tw;x2++)
                        {
                            // i is iterating over r/g/b slots for the current pixel
                            for(i=0;i<3;i++)
                            {
                                *(Data+(tw*3*y2)+x2*3+i) = pic[x*80+ts+x2][y*2+y2-y_offset][i];
                            }
                        }
                    }

                    switch(ConvertType)
                    {
                        case 0:
                            to_indexed(Data,4,0,TileWidth[x1],2);            // Median Reduction No Dither
                            break;

                        case 1:

                            to_indexed(Data,4,1,TileWidth[x1],2);            // Median Reduction With Dither
                            break;

                        case 2:
                            wuReduce(Data,4,TileWidth[x1]*2);                // Wu Reduction
                            break;
                    }

                    for(y2=0;y2<4;y2++)
                    {
                        // Skip if Y is outside allocated Palette size (prevents buffer overflow)
                        // (Left side calcs hang off top and bottom of screen
                        // due to Left/Right palette update interleaving)
                        if (y >= y_region_count_lr_rndup) continue;

                        IdealPal[x*4+x1][y][y2][0]=QuantizedPalette[y2][2];
                        IdealPal[x*4+x1][y][y2][1]=QuantizedPalette[y2][1];
                        IdealPal[x*4+x1][y][y2][2]=QuantizedPalette[y2][0];
                    }

                    for(y2=0;y2<2;y2++)
                    {
                        for(x2=0;x2<tw;x2++)
                        {
                            // Skip if Y line is outside image borders (prevents buffer overflow)
                            // since Left side calcs hang off top and bottom of image/screen
                            s32 y_line = (y*2+y2-y_offset);
                            if ((y_line < image_y_min) || (y_line > image_y_max)) continue;

                            col=Picture256[y2*tw+x2];
                            out[x*80+x2+ts][y*2+y2-y_offset]=col;

                            for(i=0;i<3;i++)
                            {
                                *(pBitsdest+(image_y_max-(y*2+y2-y_offset))*3*160+(x*80+ts+x2)*3+i)=QuantizedPalette[col][i];
                            }
                        }
                    }
                }
            }

            TempD=ImageRating(pBitssource,pBitsdest,StartX*80,StartY*8,Width*80,Height*8);

            if(TempD<BestQuantLine)
            {
                BestLine=j;
                BestQuantLine=TempD;
            }
        }
    }
    return BestLine;
}




