#include <stdint.h>

char tolower (char c) {
  return ((uint8_t)((uint8_t)c - 'A') < ('Z' - 'A' + 1)) ? c + 32u : c;
}
