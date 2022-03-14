#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

bool islower(char c) {
    return ((uint8_t)((uint8_t)c - 'a') < ('z' - 'a' + 1)) ? true : false;
}
