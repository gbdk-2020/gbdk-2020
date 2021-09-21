#ifndef __RLEDECOMPRESS_H_INCLUDE
#define __RLEDECOMPRESS_H_INCLUDE

#include <types.h>
#include <stdint.h>

#define RLE_STOP 0

#if defined(__TARGET_gb) || defined(__TARGET_ap)
uint8_t rle_init(void * data) OLDCALL;
uint8_t rle_decompress(void * dest, uint8_t len) OLDCALL; 
#elif defined(__TARGET_sms) || defined(__TARGET_gg)
uint8_t rle_init(void * data) __z88dk_fastcall;
uint8_t rle_decompress(void * dest, uint8_t len) __z88dk_callee; 
#else
  #error Unrecognized port
#endif

#endif