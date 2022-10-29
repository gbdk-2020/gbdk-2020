// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _AREAS_H
#define _AREAS_H

typedef struct area_item {
    uint32_t start;
    uint32_t end;
    uint32_t length;
} area_item;

typedef struct bank_info {
    bool     overflowed_into;
    uint16_t overflow_from;
    bool     had_multiple_write_warning;
} bank_info;

#define BANKS_MAX_COUNT  512
#define BANK_NUM(addr)  ((addr & 0xFFFFC000U) >> 14)

void areas_init(void);
void areas_cleanup(void);
int areas_add(area_item * p_area);

#endif // _AREAS_H