#include <stdio.h>

void puts(const char *s) NONBANKED
{
    while (*s)
	putchar(*s++);
#ifdef __TARGET_msxdos
    putchar('\r');
#endif
    putchar('\n');
}
