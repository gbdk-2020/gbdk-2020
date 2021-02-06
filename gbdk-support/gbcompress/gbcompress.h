// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _GBCOMPRESS_H
#define _GBCOMPRESS_H

uint32_t gbcompress_buf(uint8_t * inBuf, uint32_t InSize, uint8_t * outBuf, uint32_t MaxSize);
uint32_t gbdecompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t * outBuf, uint32_t size_max);

#endif // _GBCOMPRESS_H
