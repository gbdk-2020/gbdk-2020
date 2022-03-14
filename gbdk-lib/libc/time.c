/*
  time.c
  Simple, not completely conformant implementation of time routines
*/

#include <stdint.h>

/* clock() is in clock.s */
#include <time.h>

time_t time(time_t *t) {
    uint16_t ret;
    /* Should be relative to 0:00 1970 GMT but hey */
    ret = clock() / CLOCKS_PER_SEC;
    if (t) *t = ret;
    return ret;
}
