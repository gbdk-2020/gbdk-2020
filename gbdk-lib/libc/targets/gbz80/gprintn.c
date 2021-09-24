#include <stdint.h>
#include <gb/drawing.h>

/* Print a number in any radix */

extern char *digits;

void gprintn(int8_t number, int8_t radix, int8_t signed_value) NONBANKED
{
  uint8_t i;

  if(number < 0 && signed_value) {
    wrtchr('-');
    number = -number;
  }
  if((i = (uint8_t)number / (uint8_t)radix) != 0)
    gprintn(i, radix, UNSIGNED);
  wrtchr(digits[(uint8_t)number % (uint8_t)radix]);
}
