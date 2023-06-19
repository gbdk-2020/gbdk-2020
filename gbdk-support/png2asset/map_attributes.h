#pragma once

unsigned char GetMapAttribute(size_t x, size_t y);
void ReduceMapAttributes2x2(const vector< SetPal >& palettes);
void AlignMapAttributes();
void PackMapAttributes();