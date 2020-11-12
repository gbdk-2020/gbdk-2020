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


void areas_init(void);
void areas_cleanup(void);
int areas_add(area_item * p_area);

#endif // _AREAS_H