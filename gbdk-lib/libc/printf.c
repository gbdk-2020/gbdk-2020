#include <gbdk-lib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef void (*emitter_t)(char, char **);

static inline void _printhex(unsigned u, emitter_t emitter, char ** pData)
{
    const char *_hex = "0123456789ABCDEF";
    for (char i = 0; i < 4; i++) {
        (*emitter)(_hex[u >> 12], pData);
        u = u << 4;     
    }
}

static void _printf(const char *format, emitter_t emitter, char **pData, va_list va)
{
    while (*format) {
        if (*format == '%') {
            switch (*++format) {
                case 'c': {
                    char c = va_arg(va, char);
                    (*emitter)(c, pData);
                    break;
                }
                case 'u':
                {
                    char buf[16];
                    utoa(va_arg(va, int), buf);
                    char * s = buf;
                    while (*s) (*emitter)(*s, pData), s++;
                    break;
                }
                case 'd':
                {
                    char buf[16];
                    itoa(va_arg(va, int), buf);
                    char * s = buf;
                    while (*s) (*emitter)(*s, pData), s++;
                    break;
                }
                case 'x':
                {
                    unsigned u = va_arg(va, int);
                    _printhex(u, emitter, pData);
                    break;
                }
                case 's': 
                {
                    char *s = va_arg(va, char *);
                    while (*s) (*emitter)(*s, pData), s++;
                    break;
                }
            }
        } else {
            (*emitter)(*format, pData);
        }
        format++;
    }
}

void printf(const char *format, ...)
{
    va_list va;
    va_start(va, format);

    _printf(format, (emitter_t)putchar, NULL, va);
}

static void _sprintf_emitter(char c, char ** pData)
{
    **pData = c;
    (*pData)++;
}

void sprintf(char *into, const char *format, ...)
{
    va_list va;
    va_start(va, format);

    _printf(format, _sprintf_emitter, &into, va);
    _sprintf_emitter('\0', &into);
}
