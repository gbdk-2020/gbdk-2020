#include <gbdk-lib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef void (*emitter_t)(char, char **);

static const char _hex[] = "0123456789ABCDEF";

static inline void _printhex(unsigned int u, emitter_t emitter, char ** pData)
{
    for (char i = 0; i < 4; i++) {
        (*emitter)(_hex[u >> 12], pData);
        u = u << 4;     
    }
}

static inline void _printhexbyte(unsigned char u, emitter_t emitter, char ** pData)
{
    (*emitter)(_hex[u >> 4], pData);
    (*emitter)(_hex[u & 0x0fu], pData);
}

static void _printbuf(char * buf, emitter_t emitter, char ** pData) {
    char *s = buf;
    while (*s) (*emitter)(*s, pData), s++;
}

void __printf(const char *format, emitter_t emitter, char **pData, va_list va)
{
    char buf[16];
    while (*format) {
        if (*format == '%') {
            format++;

            // 0 Padding is not supported, ignore
            if (*format == '0') format++;

            // Width Specifier is not supported, ignore 1 digit worth
            if ((*format >= '1') && (*format <= '9')) format++;

            switch (*format) {
                case 'h': {
                    switch (*++format) {
                        case 'X' :
                        case 'x' : {
                            _printhexbyte(va_arg(va, char), emitter, pData);
                            break;
                        }
                        case 'u':
                        {
                            utoa((unsigned char)va_arg(va, char), buf);
                            _printbuf(buf, emitter, pData);
                            break;
                        }
                        case 'd':
                        {
                            itoa((signed char)va_arg(va, char), buf);
                            _printbuf(buf, emitter, pData);
                            break;
                        }
                    }
                    break;
                }
                case 'c': {
                    char c = va_arg(va, char);
                    (*emitter)(c, pData);
                    break;
                }
                case 'u':
                {
                    utoa(va_arg(va, int), buf);
                    _printbuf(buf, emitter, pData);
                    break;
                }
                case 'd':
                {
                    itoa(va_arg(va, int), buf);
                    _printbuf(buf, emitter, pData);
                    break;
                }
                case 'X':
                case 'x':
                {
                    _printhex(va_arg(va, int), emitter, pData);
                    break;
                }
                case 's': 
                {
                    _printbuf(va_arg(va, char *), emitter, pData);
                    break;
                }
            }
        } else {
            (*emitter)(*format, pData);
        }
        format++;
    }
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

    __printf(format, _sprintf_emitter, &into, va);
    _sprintf_emitter('\0', &into);
}
