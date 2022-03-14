// targets.h

#include <stdbool.h>

#ifndef _LCC_TARGETS_H
#define _LCC_TARGETS_H

// For Linker list defaults (llist0)
typedef struct arg_entry {
    char searchkey[255];
    char addflag[255];
    char addvalue[255];
    bool found;
} arg_entry;

// For Port/Platform specific settings
typedef struct CLASS {
    const char *port;
    const char *plat;
    const char *default_plat;
    const char *rom_extension;
    const char *cpp;
    const char *include;
    const char *com;
    const char *as;
    const char *bankpack;
    const char *ld;
    const char *ihxcheck;
    const char *mkbin;
    const char *postproc;
    arg_entry  *llist0_defaults;
    int         llist0_defaults_len;
} CLASS;


extern CLASS classes[];
extern int classes_count;

#endif // _TARGETS_H
