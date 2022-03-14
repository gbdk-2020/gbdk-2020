#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

bool isalpha(char c) {
    return ((((uint8_t)((uint8_t)c - 'a') < ('z' - 'a' + 1))) || ((uint8_t)((uint8_t)c - 'A') < ('Z' - 'A' + 1))) ? true : false;
}
