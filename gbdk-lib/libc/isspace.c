#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

bool isspace(char c) {
    return (((uint8_t)c == ' ') || ((uint8_t)c == '\t') || ((uint8_t)c == '\n')) ? true : false;
}
