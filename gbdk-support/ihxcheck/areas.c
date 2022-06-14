// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "areas.h"

#define AREA_GROW_SIZE 500

extern bank_info banks[];

area_item * arealist;
uint32_t    arealist_size;
uint32_t    arealist_count;


uint32_t min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

uint32_t max(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}


// Returns size of overlap between two address ranges,
// if zero then no overlap
static uint32_t addrs_check_overlap(uint32_t a_start, uint32_t a_end, uint32_t b_start, uint32_t b_end) {

    uint32_t size_used;

    // Check whether the address range *doesn't* overlap
    if ((b_start > a_end) || (b_end < a_start)) {
        size_used =  0; // no overlap, size = 0
    } else {
        size_used = min(b_end, a_end) - max(b_start, a_start) + 1; // Calculate minimum overlap

        uint32_t overlap_start = max(a_start, b_start);
        uint32_t overlap_end   = min(a_end, b_end);

        // Flag a multiple write warning in the bank where the overflow ENDS
        // Used later to check for overflows
        banks[BANK_NUM(overlap_end)].had_multiple_write_warning = true;

        printf("Warning: Multiple write of %5d bytes at 0x%x -> 0x%x writes:(0x%x -> 0x%x, 0x%x -> 0x%x)\n",
                size_used, overlap_start, overlap_end, a_start, a_end, b_start, b_end);
    }
    return size_used;
}


void arealist_additem(area_item * p_area) {

    arealist_count++;
    // Grow array if needed
    if (arealist_count == arealist_size) {
        arealist_size += AREA_GROW_SIZE;
        arealist = (area_item *)realloc(arealist, arealist_size * sizeof(area_item));
    }

    arealist[arealist_count-1] = *p_area;
}


void areas_init(void) {
    arealist_count  = 0;
    arealist_size   = AREA_GROW_SIZE;
    arealist        = (area_item *)malloc(arealist_size * sizeof(area_item));
}


void areas_cleanup(void) {
    if (arealist)
        free (arealist);
}


int areas_add(area_item * p_area) {

    uint32_t c;
    uint32_t size_used;
    int ret = true; // default to success

    // Check for overlap with existing areas
    for (c = 0; c < arealist_count; c++) {

        size_used = addrs_check_overlap(arealist[c].start, arealist[c].end,
                                        p_area->start, p_area->end);
        // Signal failure on any overlap
        // (Keep looping to display all warnings though)
        if (size_used > 0)
            ret = false;
    }

    // Now add the area
    arealist_additem(p_area);

    return ret;
}