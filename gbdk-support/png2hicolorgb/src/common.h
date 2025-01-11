// See LICENSE  file for license details

#ifndef _COMMON_H
#define _COMMON_H

#include <stdbool.h>

#define TILE_HEIGHT_PX       8
#define TILE_WIDTH_PX        8

#define MAX_STR_LEN     4096
#define DEFAULT_STR_LEN 100

#define ARRAY_LEN(A)  sizeof(A) / sizeof(A[0])

#define MAX_PATH (MAX_STR_LEN)

#ifndef __has_attribute
#define __has_attribute(attr) 0
#endif
#if __has_attribute(format)
#define FMT(kind, str_idx, first_to_check) __attribute__((format(kind, str_idx, first_to_check)))
#else
#define FMT(kind, str_idx, first_to_check)
#endif

enum {
    IMG_TYPE_PNG
};

void set_exit_error(void);
bool get_exit_error(void);

#endif // _COMMON_H
