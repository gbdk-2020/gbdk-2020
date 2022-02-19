/** Dumb strings hack.
*/
#include <string.h>
#include <gbdk/gbdk-lib.h>

#if USE_C_STRCPY
char *strcpy(char *dest, const char *source) NONBANKED
{
    char *d = dest;
    const char *s = source;
    while (*d = *s) d++, s++;
    return dest;
}
#endif

#if USE_C_MEMCPY
void *memcpy(void *dest, const void *source, size_t count) NONBANKED
{
    char *d = dest;
    const char *s = source;
    while (count--) {
        *d = *s;
        d++;
        s++;
    }
    return dest;
}
#endif

#if USE_C_STRCMP
int strcmp(const char *s1, const char *s2) {
    char ret = 0;
    while (!(ret = *s1 - *s2) && *s2) ++s1, ++s2;
    return (ret < 0) ? -1 : ((ret > 0) ? 1 : 0);
}
#endif
