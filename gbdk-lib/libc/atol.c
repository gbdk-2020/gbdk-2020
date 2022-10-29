#include <stdint.h>
#include <stdbool.h>

inline bool _isdigit(char c) {
    return ((uint8_t)((uint8_t)c - '0') < 10u) ? true : false;
}

long atol(const char *s) {
    bool sign = false;
    long n;

    const uint8_t * pc = (const uint8_t *)s;

    for(; ((*pc == ' ') || (*pc == '\t') || (*pc == '\n')); ++pc);
    
    switch(*pc) {
        case '-':
            sign = true;
            /* and fall through */
        case '+':
            ++pc;
            break;
    }
    
    for(n = 0; _isdigit(*pc); ++pc) n = 10 * n + (*pc - '0');

    return (sign == 0 ? n : -n);
}
