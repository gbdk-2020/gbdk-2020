// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "logging.h"
#include "list.h"

#define LIST_GROW_SIZE 100 // 50 // grow array by N entries at a time

// Initialize the list and it's array
// typesize *must* match the type that will be used with the array
void list_init(list_type * p_list, size_t array_typesize) {
    p_list->typesize = array_typesize;
    p_list->count   = 0;
    p_list->size    = LIST_GROW_SIZE;
    p_list->p_array = (void *)malloc(p_list->size * p_list->typesize);

    if (!p_list->p_array) {
        log_error("ERROR: Failed to allocate memory for list!\n");
        exit(EXIT_FAILURE);
    }
}


// Free the array memory allocated for the list
void list_cleanup(list_type * p_list) {
    if (p_list->p_array) {
        free (p_list->p_array);
        p_list->p_array = NULL;
    }
}


// Add a new item to the lists array, resize if needed
// p_newitem *must* be the same type the list was initialized with
// New item gets copied, so ok if it's a local var with limited lifetime
void list_additem(list_type * p_list, void * p_newitem) {

    void * tmp_list;

    p_list->count++;

    // Grow array if needed
    if (p_list->count == p_list->size) {
        // Save a copy in case reallocation fails
        tmp_list = p_list->p_array;

        p_list->size += p_list->typesize * LIST_GROW_SIZE;
        p_list->p_array = (void *)realloc(p_list->p_array, p_list->size * p_list->typesize);
        // If realloc failed, free original buffer before quitting
        if (!p_list->p_array) {
            log_error("ERROR: Failed to reallocate memory for list!\n");
            if (tmp_list) {
                free(tmp_list);
                tmp_list = NULL;
            }
            exit(EXIT_FAILURE);
        }
    }

    // Copy new entry
    memcpy((uint_least8_t *)p_list->p_array + ((p_list->count - 1) * p_list->typesize),
           p_newitem,
           p_list->typesize);
}

