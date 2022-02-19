#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

bool isdigit(char c) {
    return ((uint8_t)((uint8_t)c - '0') < 10u) ? true : false;
}
