/***************************************************************************
 *                                                                         *
 * Module  : rand.c                                                        *
 *                                                                         *
 * Purpose : A test for the rand() function, for the GBDK                  *
 *                                                                         *
 * Version : 1, Januari 6 1998                                             *
 *                                                                         *
 * Author  : Luc Van den Borre ( Homepage : NOC.BASE.ORG )                 *
 *                                                                         *
 **************************************************************************/

#include <gb/gb.h>
#include <rand.h>
#include <gb/drawing.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define RANGE_SIZE       (160u / 4u)
#define HALF_RANGE_SIZE  (RANGE_SIZE / 2u)

UBYTE accu[RANGE_SIZE];
UBYTE accua[RANGE_SIZE];
UBYTE accut[RANGE_SIZE];
UBYTE accub[RANGE_SIZE];

// Fast alternative to modulo for arbitrary 8 bit range sizes
// (range sizes that are exact powers of 2 can use faster bit shift or mask reductions)
//
// see: https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
//
// Max value for RANGE is 256
#define RAND_RANGE_8BIT(randval, range) (uint8_t)(((randval & 0xFFu) * range) >> 8)

void main(void)
{
    uint16_t seed;

    memset(accu, 0, sizeof(accu));
    memset(accua, 0, sizeof(accua));
    memset(accut, 0, sizeof(accut));
    memset(accut, 0, sizeof(accub));

    /* We use the DIV register to get a random initial seed */
    puts("Getting seed");
    puts("Push any key (1)");
    waitpad(0xFF);
    waitpadup();
    seed = DIV_REG;
    puts("Push any key (2)");
    waitpad(0xFF);
    waitpadup();
    seed |= (UWORD)DIV_REG << 8;

    // initarand() calls initrand()
    initarand(seed);

    // Draw some divider lines
    line(RANGE_SIZE * 1, 0, RANGE_SIZE * 1, 143);
    line(RANGE_SIZE * 2, 0, RANGE_SIZE * 2, 143);
    line(RANGE_SIZE * 3, 0, RANGE_SIZE * 3, 143);

    while (1) {

        // Generate standard random values in the range of 0 .. RANGE_SIZE - 1
        // and accumulate the values into buckets, then plot the bucket sizes

        // Using rand()
        uint8_t r = RAND_RANGE_8BIT(rand(), RANGE_SIZE);

        // Using arand()
        uint8_t ra = RAND_RANGE_8BIT(arand(), RANGE_SIZE);

        // Create a triangle distribution using (rand - rand)
        uint8_t rt1 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE);
        uint8_t rt2 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE);
        uint8_t rt = HALF_RANGE_SIZE + (rt1 - rt2);

        // Create a bell-curve-ish distribution using (rand - rand) + (rand - rand)
        uint8_t rb1 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE / 2);
        uint8_t rb2 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE / 2);
        uint8_t rb3 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE / 2);
        uint8_t rb4 = RAND_RANGE_8BIT(rand(), HALF_RANGE_SIZE / 2);
        uint8_t rb = HALF_RANGE_SIZE + (rb1 - rb2) + (rb3 - rb4);


        // Plot the updated rand value buckets
        //
        // rand | arand | rand with triangle distribution | bell-curve-ish

        uint8_t r_bucket_height = ++accu[r];
        if (r_bucket_height > 144) break;
        plot(r  + (RANGE_SIZE * 0), 144-r_bucket_height, LTGREY, SOLID);

        uint8_t ra_bucket_height = ++accua[ra];
        if (ra_bucket_height > 144) break;
        plot(ra + (RANGE_SIZE * 1), 144 - ra_bucket_height, DKGREY, SOLID);

        uint8_t rt_bucket_height = ++accut[rt];
        if (rt_bucket_height > 144) break;
        plot(rt + (RANGE_SIZE * 2), 144 - rt_bucket_height, BLACK, SOLID);

        uint8_t rb_bucket_height = ++accub[rb];
        if (rb_bucket_height > 144) break;
        plot(rb + (RANGE_SIZE * 3), 144 - rb_bucket_height, BLACK, SOLID);
    }
}

