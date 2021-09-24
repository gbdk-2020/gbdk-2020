// list.c

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <assert.h>
//#include <ctype.h>
//#include <signal.h>

#include "list.h"

// From lcc.c
extern void *alloc(int);
extern char *strsave(const char *str);
extern char *stringf(const char *fmt, ...);
extern char *path_stripext(char *name);
extern int matches_ext(const char * filename, const char * ext);
extern int verbose;



/* append - append a node with string str onto list, return new list */
List append(char *str, List list) {
    List p = alloc(sizeof *p);

    p->str = str;
    if (list) {
        p->link = list->link;
        list->link = p;
    }
    else
        p->link = p;
    return p;
}


/* find - find 1st occurrence of str in list, return list node or 0 */
List find(char *str, List list) {
    List b;

    b = list;
    if (b)
        do {
            if (strcmp(str, b->str) == 0)
                return b;
        } while ((b = b->link) != list);
        return 0;
}


/* path2list - convert a colon- or semicolon-separated list to a list */
List path2list(const char *path) {
    List list = NULL;
    char sep = ':';

    if (path == NULL)
        return NULL;
    if (strchr(path, ';'))
        sep = ';';
    while (*path) {
        char *p, buf[512];
        p = strchr(path, sep);
        if (p) {
            size_t len = p - path;
            if(len >= sizeof(buf)) len = sizeof(buf)-1;
            strncpy(buf, path, len);
            buf[len] = '\0';
        }
        else {
            strncpy(buf, path, sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
        }
        if (!find(buf, list))
            list = append(strsave(buf), list);
        if (p == 0)
            break;
        path = p + 1;
    }
    return list;
}


// Replace extensions for filenames in a list
void list_rewrite_exts(List list_in, char * ext_match, char * ext_new)
{
    char * filepath_old;

    // Iterate through list and replace file extensions
    if (list_in) {
        List list_t = list_in;
        do {
            if (list_t->str) {
                // Check to see if filname has desired extension
                if (matches_ext(list_t->str, ext_match)) {

                    // Save a copy to free after re-assignment
                    filepath_old = list_t->str;
                    // Create a new string with the replaced suffix (stringf() allocs)
                    list_t->str = stringf("%s%s", path_stripext(list_t->str), ext_new);
                    if (verbose > 0) fprintf(stderr,"lcc: rename link obj (from -autobank): %s -> %s\n", filepath_old, list_t->str);
                }
            }
            // Move to next list item, exit if start of list is reached
            list_t = list_t->link;
        } while (list_t != list_in);
    }
}


// Duplicate items with [ext_match] into new list items with [ext_new]
void list_duplicate_to_new_exts(List list_in, char * ext_match, char * ext_new)
{
    // List may have entries appended, cache original start
    List list_start = list_in;

    // Iterate through list and replace file extensions
    if (list_in) {
        List list_t = list_in;
        do {
            if (list_t->str) {
                // Check to see if filname has desired extension
                if (matches_ext(list_t->str, ext_match)) {
                    // Make a copy with the replaced extension into the list
                    list_in = append(stringf("%s%s", path_stripext(list_t->str), ext_new), list_in);
                    if (verbose > 0) fprintf(stderr,"lcc: add to rmlist (from -autobank): %s -> %s\n", list_t->str, stringf("%s%s", path_stripext(list_t->str), ext_new));
                }
            }
            // Move to next list item, exit if start of list is reached
            list_t = list_t->link;
        } while (list_t != list_start);
    }
}


// Remove all items from a list
List list_remove_all(List list_in) {

    // Well... the custom alloc() used for lists makes it hard to free their memory.
    // Instead just set the list to NULL to erase it and let cleanup happen on program exit.
    return NULL;

    /*
    List list_next;
    // Iterate through list and free memory
    if (list_in) {
        List list_t = list_in->link; // Advance to next item
        list_in->link = NULL; // Break list to create a stopping point

        do {
            // Copy next link before freeing the current item
            list_next = list_t->link;
            if (list_t) {
                free(list_t);
                list_t = NULL;
            }
            // Move to next list item, exit if start of list is reached
            list_t = list_next;
        } while (list_t && list_t->link);
    }
    return NULL;
    */
}


// Copies all items from [list_src] to [list_dest]
// * optionally prefix items from [list_src] with [str_prefix]
// * optionally append() a separate [str_add_before] for each item in [list_src] (use for space separated flags)
List list_add_to_another(List list_dest, List list_src, char * str_prefix, char * str_add_before) {

    if (list_src) {
        List list_t = list_src->link; // Start at first list item (list usually points to END)

        // Iterate through [list_src] and copy into to [list_dest], exit once start is reached again
        do {
            if (list_t->str)
                // Add str_add_before string as a separate item if present
                if (str_add_before)
                    list_dest = append( stringf("%s", str_add_before), list_dest);

                // pre-pend string prefix if present
                if (str_prefix)
                    list_dest = append( stringf("%s%s", str_prefix, list_t->str), list_dest);
                else
                    list_dest = append( stringf("%s", list_t->str), list_dest);
            // Move to next list item
            list_t = list_t->link;
        } while (list_t != list_src);
    }

    return list_dest; // Return updated list
}
