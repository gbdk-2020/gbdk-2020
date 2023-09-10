#ifndef TILE_DEDUPE_H
#define TILE_DEDUPE_H

#include <stdint.h>
#include <stdbool.h>

bool tileset_find_matching_tile(const uint8_t * src_tile, const uint8_t * tile_set_deduped, unsigned int tile_count_deduped, uint8_t * new_tile_id, uint8_t * new_attribs);

#endif

