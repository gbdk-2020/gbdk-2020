#include <stdint.h>
#include <gb/drawing.h>

/* Print a long number in any radix */

extern char *digits;

void gprintln(int16_t number, int8_t radix, int8_t signed_value) NONBANKED
{
  uint16_t l;

  if(number < 0 && signed_value) {
    wrtchr('-');
    number = -number;
  }
  if((l = (uint16_t)number / (uint16_t)radix) != 0)
    gprintln(l, radix, UNSIGNED);
  wrtchr(digits[(uint16_t)number % (uint16_t)radix]);
}
