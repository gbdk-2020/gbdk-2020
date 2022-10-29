#include <stdio.h>

extern const char *__crlf;

void puts(const char *s) NONBANKED
{
    while (*s)
        putchar(*s++);

    for (s = __crlf; (*s); )
        putchar(*s++);
}
