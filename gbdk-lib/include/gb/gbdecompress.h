// GB-Compress decompressor
// Compatible with GBTD

#ifndef __GBDECOMPRESS_H_INCLUDE
#define __GBDECOMPRESS_H_INCLUDE

/** gb-decompress data from sour into dest
 */
void gb_decompress(const unsigned char * sour, unsigned char * dest) __preserves_regs(b, c);

/** gb-decompress background tiles into VRAM
 */
void gb_decompress_bkg_data(UINT8 first_tile, const unsigned char * sour) __preserves_regs(b, c);

/** gb-decompress window tiles into VRAM
 */
void gb_decompress_win_data(UINT8 first_tile, const unsigned char * sour) __preserves_regs(b, c);

/** gb-decompress sprite tiles into VRAM
 */
void gb_decompress_sprite_data(UINT8 first_tile, const unsigned char * sour) __preserves_regs(b, c);

#endif