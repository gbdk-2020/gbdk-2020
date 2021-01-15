// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _PATH_OPS_H
#define _PATH_OPS_H

void filename_replace_extension(char * filename, char * new_ext, size_t maxlen);
void filename_replace_path(char * filename, char * new_path, size_t maxlen);

#endif // _PATH_OPS_H