#include <gbdk/platform.h>
#include <stdint.h>

#ifndef _MEGADUCK_MODEL_H
#define _MEGADUCK_MODEL_H

#define MEGADUCK_HANDHELD_STANDARD 0u
#define MEGADUCK_LAPTOP_SPANISH    1u
#define MEGADUCK_LAPTOP_GERMAN     2u


/** Returns which MegaDuck Model the program is being run on

    Possible models are:
    - Handheld: @ref MEGADUCK_HANDHELD_STANDARD
    - Spanish Laptop "Super QuiQue": @ref MEGADUCK_LAPTOP_SPANISH
    - German Laptop "Super Junior Computer": @ref MEGADUCK_LAPTOP_GERMAN

    This detection should be called immediately at the start of the program
    for most reliable results, since it relies on inspecting uncleared VRAM
    contents.

    It works by checking for distinct font VRAM Tile Patterns (which aren't
    cleared before cart program launch) between the Spanish and German Laptop
    models which have slightly different character sets.

    So VRAM *must not* be cleared or modified at program startup until after
    this function is called (not by the crt0.s, not by the program itself).

    @note This detection may not work in emulators which don't simulate
          the preloaded Laptop System ROM font tiles in VRAM.
*/
uint8_t duck_check_model(void);

#endif // _MEGADUCK_MODEL_H
