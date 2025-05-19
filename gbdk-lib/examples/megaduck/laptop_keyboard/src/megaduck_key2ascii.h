#include <gbdk/platform.h>
#include <stdint.h>

#ifndef _MEGADUCK_KEY2ASCII_H
#define _MEGADUCK_KEY2ASCII_H

char duck_io_scancode_to_ascii(const uint8_t key_code, const uint8_t megaduck_model);

#endif // _MEGADUCK_KEY2ASCII_H
