#include <stdint.h>

char toupper(char c) {
  return ((uint8_t)((uint8_t)c - 'a') < ('z' - 'a' + 1)) ? c - 32u : c;
}
