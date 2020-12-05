// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _LIST_H
#define _LIST_H


typedef struct list_type {
    void *   p_array;
    uint32_t size;
    uint32_t count;
    size_t   typesize;
} list_type;

void list_init(list_type *, size_t);
void list_cleanup(list_type *);
void list_additem(list_type *, void *);

#endif // _LIST_H