// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _PATH_OPS_H
#define _PATH_OPS_H

#include <stdint.h>
#include <stdbool.h>

void filename_replace_extension(char * filename, char * new_ext, size_t maxlen);
void filename_replace_path(char * filename, char * new_path, size_t maxlen);
const char * get_filename_from_path(const char * path);
void filename_remove_extension(char * path);
bool get_path_without_filename(const char * path, char * path_only, uint32_t str_max);
bool matches_extension(char *, char *);

#endif // _PATH_OPS_H