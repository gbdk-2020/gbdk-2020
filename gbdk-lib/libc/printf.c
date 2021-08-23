#include <stdio.h>
#include <stdarg.h>

void __printf(const char *format, void *emitter, char **pData, va_list va);

void printf(const char *format, ...)
{
    va_list va;
    va_start(va, format);

    __printf(format, (void *)putchar, NULL, va);
}
