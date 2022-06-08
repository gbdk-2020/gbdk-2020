#include <types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

typedef void (*emitter_t)(char, char **) OLDCALL REENTRANT;

static const char _hex[] = "0123456789ABCDEF";

inline void _printhex(uint16_t u, emitter_t emitter, char ** pData)
{
    (*emitter)(_hex[(uint8_t)(u >> 8) >> 4], pData);
    (*emitter)(_hex[(uint8_t)(u >> 8) & 0x0fu], pData);
    (*emitter)(_hex[((uint8_t)u >> 4) & 0x0fu], pData);
    (*emitter)(_hex[(uint8_t)u & 0x0fu], pData);
}

inline void _printhexbyte(uint8_t u, emitter_t emitter, char ** pData)
{
    (*emitter)(_hex[u >> 4], pData);
    (*emitter)(_hex[u & 0x0fu], pData);
}

static void _printbuf(char * buf, emitter_t emitter, char ** pData) {
    for (char *s = buf; *s; s++) (*emitter)(*s, pData);
}

void __printf(const char *format, emitter_t emitter, char **pData, va_list va)
{
    char buf[16];
    while ((uint8_t)(*format)) {
        if ((uint8_t)(*format) == '%') {
            format++;

            // 0 Padding is not supported, ignore
            if ((uint8_t)(*format) == '0') format++;

            // Width Specifier is not supported, ignore 1 digit worth
            if ((uint8_t)((uint8_t)(*format) - '1') < 9u) format++;

            switch ((uint8_t)(*format)) {
                case 'h': {
                    switch ((uint8_t)(*++format)) {
                        case 'X' :
                        case 'x' : {
                            _printhexbyte(va_arg(va, char), emitter, pData);
                            break;
                        }
                        case 'u':
                        {
                            uitoa((unsigned char)va_arg(va, char), buf, 10);
                            _printbuf(buf, emitter, pData);
                            break;
                        }
                        case 'd':
                        {
                            itoa((signed char)va_arg(va, char), buf, 10);
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
                    uitoa(va_arg(va, int), buf, 10);
                    _printbuf(buf, emitter, pData);
                    break;
                }
                case 'd':
                {
                    itoa(va_arg(va, int), buf, 10);
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

static void _sprintf_emitter(char c, char ** pData) OLDCALL REENTRANT {
    **pData = c;
    (*pData)++;
}

void sprintf(char *into, const char *format, ...) OLDCALL {
    va_list va;
    va_start(va, format);

    __printf(format, _sprintf_emitter, &into, va);
    _sprintf_emitter('\0', &into);
}
