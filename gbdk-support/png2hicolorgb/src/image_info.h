//
// image_info.h
//

#ifndef IMAGE_INFO_HEADER
#define IMAGE_INFO_HEADER

#include <stdint.h>

    #define RGBA_32SZ   4  // 4 bytes
    #define RGBA_RED   0
    #define RGBA_GREEN 1
    #define RGBA_BLUE  2
    #define RGBA_ALPHA 3

    #define RGB_24SZ   3  // 3 bytes
    #define RGB_RED   0
    #define RGB_GREEN 1
    #define RGB_BLUE  2

    #define BGRX_BLUE  0
    #define BGRX_GREEN 1
    #define BGRX_RED   2

    #define MODE_8_BIT_INDEXED        ( 8 / 8)
    #define MODE_8_BIT_INDEXED_ALPHA  (16 / 8)
    #define MODE_24_BIT_RGB           (24 / 8)
    #define MODE_32_BIT_RGBA          (32 / 8)

    #define COLOR_DATA_PAL_MAX_COUNT   256 // 255 Colors max for indexed
    #define COLOR_DATA_BYTES_PER_COLOR 3   // RGB 1 byte per color
    #define COLOR_DATA_PAL_SIZE COLOR_DATA_PAL_MAX_COUNT * COLOR_DATA_BYTES_PER_COLOR

    #define USER_PAL_MAX_COLORS  32


    typedef struct {
        uint8_t    bytes_per_pixel;
        // uint16_t   width;
        // uint16_t   height;
        unsigned int   width;
        unsigned int   height;
        uint32_t   size;        // size in bytes
        uint8_t  * p_img_data;
        uint16_t   tile_width;  // should be even multiple of width 
        uint16_t   tile_height; // should be even multiple of height

        uint16_t   palette_tile_width;  // should be even multiple of width 
        uint16_t   palette_tile_height; // should be even multiple of height
    } image_data;


#endif
