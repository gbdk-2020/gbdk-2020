// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "path_ops.h"

const char kExtensionSeparator = '.';
const char kPathSeparator =

#ifdef _WIN32
  #ifndef _WIN32
     #define __WIN32__
  #endif
#endif

#ifdef __WIN32__
                            '\\';
#else
                            '/';
#endif

const char kPathSeparator_unix = '/';


void filename_replace_extension(char * filename, char * new_ext, size_t maxlen) {

    // Use a temp work string in case out and in filename are the same pointer
    char temp[MAX_PATH];
    char ext_sep[2] = {'\0'}; // default to empty string

    // Add leading . to path if needed
    if (new_ext[0] != kExtensionSeparator) {
        ext_sep[0] = kExtensionSeparator;
        ext_sep[1] = '\0';
    }

    // Strip extension from filename, append new extension
    filename_remove_extension(filename);
    snprintf(temp, maxlen, "%s%s%s", filename, ext_sep, new_ext);
    snprintf(filename, maxlen, "%s", temp);
}


void filename_replace_path(char * filename, char * new_path, size_t maxlen) {

    // Use a temp work string in case out and in filename are the same pointer
    char temp[MAX_PATH];
    char path_sep[2] = {'\0'}; // default to empty string

    // Add trailing slash to path if needed (Windows needs both for when running under linix like env)
#ifdef __WIN32__
    if (((new_path[(strlen(new_path)-1)] != kPathSeparator)) &&
       ((new_path[(strlen(new_path)-1)] != kPathSeparator_unix)))
#else
    if ((new_path[(strlen(new_path)-1)] != kPathSeparator))
#endif
    {
        path_sep[0] = kPathSeparator;
        path_sep[1] = '\0';
    }

    // Strip path from path+filename, pre-pend new path
    snprintf(temp, maxlen, "%s%s%s", new_path, path_sep, get_filename_from_path(filename));
    snprintf(filename, maxlen, "%s", temp);
}


const char * get_filename_from_path(const char * path)
{
    size_t i;

    // Returns string starting at last occurrance of path separator char
    for(i = strlen(path) - 1; i; i--) {

    // Add trailing slash to path if needed (Windows needs both for when running under linix like env)
    #ifdef __WIN32__
        if ((path[i] == kPathSeparator) || (path[i] == kPathSeparator_unix))
    #else
        if (path[i] == kPathSeparator)
    #endif
        {
            return &path[i+1];
        }
    }
    return path;
}


void filename_remove_extension(char * path)
{
    char * last_ext;
    char * last_slash;

    // Find the last path separator if present
    // Starting from here ensures that no path ".." characters
    // get mistaken as extension delimiters.
    last_slash = strrchr (path, kExtensionSeparator);
    if (!last_slash)
        last_slash = path;

    // Then check to see if there is an extension (starting with the last occurance of '.')
    // (tries to remove *all* trailing extensions backward until the last slash)
    last_ext = strrchr (last_slash, kExtensionSeparator);
    while (last_ext) {
        if (last_ext != NULL) {
            // If an extension is found then overwrite it with a string terminator
            *last_ext = '\0';
        }
        last_ext = strrchr (last_slash, kExtensionSeparator);
    }
}


bool get_path_without_filename(const char * path, char * path_only, uint32_t str_max)
{
    size_t i;

   if (strlen(path) + 1 > str_max)
        return false;

   for(i = strlen(path) - 1; i; i--) {

    // Add trailing slash to path if needed (Windows needs both for when running under linix like env)
    #ifdef __WIN32__
        if ((path[i] == kPathSeparator) || (path[i] == kPathSeparator_unix))
    #else
        if (path[i] == kPathSeparator)
    #endif
        {
            memcpy(path_only, path, i+1 );
            path_only[i+1] = '\0';
            return true;
        }
    }

    // memcpy(path_only, path, strlen(path));
    // No separater found, so no path
    path_only[0] = '\0';
    return true;
}


// Case insensitive
bool matches_extension(char * filename, char * extension) {

    if (strlen(filename) >= strlen(extension)) {
        char * str_ext = filename + (strlen(filename) - strlen(extension));

        return (strncasecmp(str_ext, extension, strlen(extension)) == 0);
    }
    else
        return false;
}
