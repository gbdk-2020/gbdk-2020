/*
       C Implementation of Wu's Color Quantizer (v2.0)
       -- Xiaolin Wu.
       From the Graphics Gems.

       Memory hungry.

       Some fixes and additional code by Benny.
*/
/**********************************************************************
        C Implementation of Wu's Color Quantizer (v. 2)
        (see Graphics Gems vol. II, pp. 126-133)

Author: Xiaolin Wu
    Dept. of Computer Science
    Univ. of Western Ontario
    London, Ontario N6A 5B7
    wu@csd.uwo.ca

Algorithm: Greedy orthogonal bipartition of RGB space for variance
       minimization aided by inclusion-exclusion tricks.
       For speed no nearest neighbor search is done. Slightly
       better performance can be expected by more sophisticated
       but more expensive versions.

The author thanks Tom Lane at Tom_Lane@G.GP.CS.CMU.EDU for much of
additional documentation and a cure to a previous bug.

Free to distribute, comments and suggestions are appreciated.
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "hicolour.h"
#include "median.h"
#include "wu.h"


#define NOINVERT        // RGB or BGR

//void            error(char *s);


#define MAXCOLOR    256
#define RED         2
#define GREEN       1
#define BLUE        0


/* Histogram is in elements 1..HISTSIZE along each axis,
  element 0 is for base or marginal value.
  NB: these must start out 0!   */
s32     wt[BOX][BOX][BOX],mr[BOX][BOX][BOX],mg[BOX][BOX][BOX],mb[BOX][BOX][BOX];
float   m2[BOX][BOX][BOX];

s32     ImageSize;           /* image size */
s32     PalSize;             /* color look-up table size */

u16     *Qadd;          // *must* be unsigned?
u8      *TrueColorPic;


// s16      AQadd[7*8*3*2];  // Signed in original code, type clash with QAdd and bit shifting
u16     AQadd[7*8*3*2];
u8      Atag[BOX * BOX * BOX];


/* build 3-D color histogram of counts, r/g/b, c^2 */
void Hist3d(s32 *vwt,s32 *vmr,s32 *vmg,s32 *vmb, float *m_2)
{
    s32     ind,r,g,b;
    s32     inr,ing,inb,table[256];
    s32     i;

    for (i = 0; i < 256; ++i)
        table[i] = i * i;

    for (i = 0; i < ImageSize; ++i)
    {
        r = TrueColorPic[i*3  ];
        g = TrueColorPic[i*3+1];
        b = TrueColorPic[i*3+2];

        inr = (r >> 3) + 1;
        ing = (g >> 3) + 1;
        inb = (b >> 3) + 1;
        Qadd[i] = ind = (inr << 10) + (inr << 6) + inr + (ing << 5) + ing + inb;
        ++vwt[ind];
        vmr[ind] += r;
        vmg[ind] += g;
        vmb[ind] += b;
        m_2[ind] += (float) (table[r] + table[g] + table[b]);
    }
}

/* At conclusion of the histogram step, we can interpret
 *   wt[r][g][b] = sum over voxel of P(c)
 *   mr[r][g][b] = sum over voxel of r*P(c)  ,  similarly for mg, mb
 *   m2[r][g][b] = sum over voxel of c^2*P(c)
 * Actually each of these should be divided by 'size' to give the usual
 * interpretation of P() as ranging from 0 to 1, but we needn't do that here.
 */

/* We now convert histogram into moments so that we can rapidly calculate
 * the sums of the above quantities over any desired box.
 */

void Momt3d(s32 *vwt, s32 *vmr, s32 *vmg, s32 *vmb, float *m_2)
{
    u16     ind1,ind2;
    u8      i,r,g,b;
    s32     line,line_r,line_g,line_b,area[BOX],area_r[BOX],area_g[BOX],area_b[BOX];
    float   line2,area2[BOX];

    for (r = 1; r <= 32; ++r)
    {
        for (i = 0; i <= 32; ++i)
        {
            area[i] = area_r[i] = area_g[i] = area_b[i] = 0;
            area2[i] = 0.0;
        }

        for (g = 1; g <= 32; ++g)
        {
            line2 = 0.0;
            line = line_r = line_g = line_b = 0;

            for (b = 1; b <= 32; ++b)
            {
                ind1 = (r << 10) + (r << 6) + r + (g << 5) + g + b;
                line += vwt[ind1];
                line_r += vmr[ind1];
                line_g += vmg[ind1];
                line_b += vmb[ind1];
                line2 += m_2[ind1];
                area[b] += line;
                area_r[b] += line_r;
                area_g[b] += line_g;
                area_b[b] += line_b;
                area2[b] += line2;
                ind2 = ind1 - 1089;
                vwt[ind1] = vwt[ind2] + area[b];
                vmr[ind1] = vmr[ind2] + area_r[b];
                vmg[ind1] = vmg[ind2] + area_g[b];
                vmb[ind1] = vmb[ind2] + area_b[b];
                m_2[ind1] = m_2[ind2] + area2[b];
            }
        }
    }
}

s32 Vol(struct box * cube, s32 mmt[BOX][BOX][BOX])
{
    return (mmt[cube->r1][cube->g1][cube->b1] - mmt[cube->r1][cube->g1][cube->b0] - mmt[cube->r1][cube->g0][cube->b1]
          + mmt[cube->r1][cube->g0][cube->b0] - mmt[cube->r0][cube->g1][cube->b1] + mmt[cube->r0][cube->g1][cube->b0]
          + mmt[cube->r0][cube->g0][cube->b1] - mmt[cube->r0][cube->g0][cube->b0]);
}

/* The next two routines allow a slightly more efficient calculation
 * of Vol() for a proposed subbox of a given box.  The sum of Top()
 * and Bottom() is the Vol() of a subbox split in the given direction
 * and with the specified new upper bound.
 */

/* Compute part of Vol(cube, mmt) that doesn't depend on r1, g1, or b1 */
/* (depending on dir) */
s32 Bottom(struct box * cube, u8 dir, s32 mmt[BOX][BOX][BOX])
{
    switch (dir)
    {

        case RED:

            return (-mmt[cube->r0][cube->g1][cube->b1] + mmt[cube->r0][cube->g1][cube->b0]
                    + mmt[cube->r0][cube->g0][cube->b1] - mmt[cube->r0][cube->g0][cube->b0]);
        case GREEN:

            return (-mmt[cube->r1][cube->g0][cube->b1] + mmt[cube->r1][cube->g0][cube->b0]
                    + mmt[cube->r0][cube->g0][cube->b1] - mmt[cube->r0][cube->g0][cube->b0]);

        case BLUE:

            return (-mmt[cube->r1][cube->g1][cube->b0] + mmt[cube->r1][cube->g0][cube->b0]
                    + mmt[cube->r0][cube->g1][cube->b0] - mmt[cube->r0][cube->g0][cube->b0]);
    }

    return 0;
}


s32 Top(struct box * cube, u8 dir, s32 pos, s32 mmt[BOX][BOX][BOX])
{
    switch (dir)
    {

        case RED:

            return (mmt[pos][cube->g1][cube->b1] - mmt[pos][cube->g1][cube->b0]
                  - mmt[pos][cube->g0][cube->b1] + mmt[pos][cube->g0][cube->b0]);

        case GREEN:

            return (mmt[cube->r1][pos][cube->b1] - mmt[cube->r1][pos][cube->b0]
                  - mmt[cube->r0][pos][cube->b1] + mmt[cube->r0][pos][cube->b0]);

        case BLUE:

            return (mmt[cube->r1][cube->g1][pos] - mmt[cube->r1][cube->g0][pos]
                  - mmt[cube->r0][cube->g1][pos] + mmt[cube->r0][cube->g0][pos]);
    }

    return 0;
}

/* Compute the weighted variance of a box */
/* NB: as with the raw statistics, this is really the variance * size */
float Var(struct box * cube)
{
    float   dr,dg,db,xx;

    dr = (float)Vol(cube, mr);
    dg = (float)Vol(cube, mg);
    db = (float)Vol(cube, mb);
    xx = m2[cube->r1][cube->g1][cube->b1] - m2[cube->r1][cube->g1][cube->b0] - m2[cube->r1][cube->g0][cube->b1]
        + m2[cube->r1][cube->g0][cube->b0] - m2[cube->r0][cube->g1][cube->b1] + m2[cube->r0][cube->g1][cube->b0]
        + m2[cube->r0][cube->g0][cube->b1] - m2[cube->r0][cube->g0][cube->b0];

    return (xx - (dr * dr + dg * dg + db * db) / (float) Vol(cube, wt));
}

/* We want to minimize the sum of the variances of two subboxes.
 * The sum(c^2) terms can be ignored since their sum over both subboxes
 * is the same (the sum for the whole box) no matter where we split.
 * The remaining terms have a minus sign in the variance formula,
 * so we drop the minus sign and MAXIMIZE the sum of the two terms.
 */

float Maximize(struct box *cube, u8 dir, s32 first, s32 last, s32 *cut, s32 whole_r, s32 whole_g, s32 whole_b, s32 whole_w)
{
    s32     half_r,half_g,half_b,half_w;
    s32     base_r,base_g,base_b,base_w;
    s32     i;
    float   temp,max;

    base_r = Bottom(cube, dir, mr);
    base_g = Bottom(cube, dir, mg);
    base_b = Bottom(cube, dir, mb);
    base_w = Bottom(cube, dir, wt);

    max = 0.0;
    *cut = -1;

    for (i = first; i < last; ++i)
    {
        half_r = base_r + Top(cube, dir, i, mr);
        half_g = base_g + Top(cube, dir, i, mg);
        half_b = base_b + Top(cube, dir, i, mb);
        half_w = base_w + Top(cube, dir, i, wt);
        //  now half_x is sum over lower half of box, if split at i
        if (half_w == 0)
        {
            // subbox could be empty of pixels!
            // never split into an empty box
            continue;
        }
        else
        {
            temp = ((float) half_r * half_r + (float) half_g * half_g + (float) half_b * half_b) / half_w;
        }

        half_r = whole_r - half_r;
        half_g = whole_g - half_g;
        half_b = whole_b - half_b;
        half_w = whole_w - half_w;

        if (half_w == 0)
        {
      // subbox could be empty of pixels!
      // never split into an empty box
            continue;
        }
        else
        {
            temp += ((float) half_r * half_r + (float) half_g * half_g + (float) half_b * half_b) / half_w;
        }

        if (temp > max)
        {
            max = temp;
            *cut = i;
        }
    }

    return (max);
}

s32 Cut(struct box * set1, struct box * set2)
{
    u8      dir;
    s32     cutr,cutg,cutb;
    float   maxr,maxg,maxb;
    s32     whole_r,whole_g,whole_b,whole_w;

    whole_r = Vol(set1, mr);
    whole_g = Vol(set1, mg);
    whole_b = Vol(set1, mb);
    whole_w = Vol(set1, wt);

    maxr = Maximize(set1, RED, set1->r0 + 1, set1->r1, &cutr, whole_r, whole_g, whole_b, whole_w);
    maxg = Maximize(set1, GREEN, set1->g0 + 1, set1->g1, &cutg, whole_r, whole_g, whole_b, whole_w);
    maxb = Maximize(set1, BLUE, set1->b0 + 1, set1->b1, &cutb, whole_r, whole_g, whole_b, whole_w);

    if ((maxr >= maxg) && (maxr >= maxb))
    {
        dir = RED;

        if (cutr < 0)
            return 0;           /* can't split the box */
    }
    else if ((maxg >= maxr) && (maxg >= maxb))
    {
        dir = GREEN;
    }
    else
    {
        dir = BLUE;
    }

    set2->r1 = set1->r1;
    set2->g1 = set1->g1;
    set2->b1 = set1->b1;

    switch (dir)
    {

        case RED:

            set2->r0 = set1->r1 = cutr;
            set2->g0 = set1->g0;
            set2->b0 = set1->b0;
            break;

        case GREEN:

            set2->g0 = set1->g1 = cutg;
            set2->r0 = set1->r0;
            set2->b0 = set1->b0;
            break;

        case BLUE:

            set2->b0 = set1->b1 = cutb;
            set2->r0 = set1->r0;
            set2->g0 = set1->g0;
            break;
    }

    set1->vol = (set1->r1 - set1->r0) * (set1->g1 - set1->g0) * (set1->b1 - set1->b0);
    set2->vol = (set2->r1 - set2->r0) * (set2->g1 - set2->g0) * (set2->b1 - set2->b0);

    return 1;
}


void Mark(struct box *cube, s32 label, u8 *tag)
{
    s32     r,g,b;

    for (r = cube->r0 + 1; r <= cube->r1; ++r)
        for (g = cube->g0 + 1; g <= cube->g1; ++g)
            for (b = cube->b0 + 1; b <= cube->b1; ++b)
                tag[(r << 10) + (r << 6) + r + (g << 5) + g + b] = label;
}



s32 wuReduce(u8 *RGBpic, s32 numcolors, s32 picsize)
{
    struct box  cube[MAXCOLOR];
    u8          *tag = 0;
    float       vv[MAXCOLOR],temp = 0.;
    s32         i = 0,weight = 0;
    s32         next = 0;
    s32         j = 0,k = 0,l = 0;

    TrueColorPic = RGBpic;
    ImageSize = picsize;
    PalSize = numcolors;

    for (j=0;j<BOX;j++)
        for (k=0;k<BOX;k++)
            for (l=0;l<BOX;l++)
            {
                wt[j][k][l] = 0;
                mr[j][k][l] = 0;
                mg[j][k][l] = 0;
                mb[j][k][l] = 0;
                m2[j][k][l] = 0.;
            }

    Qadd = AQadd;

    // Hist3d((long *)&wt, (long *)&mr, (long *)&mg, (long *)&mb, (float *)&m2);
    // Momt3d((long *)&wt, (long *)&mr, (long *)&mg, (long *)&mb, (float *)&m2);
    Hist3d((s32 *)&wt, (s32 *)&mr, (s32 *)&mg, (s32 *)&mb, (float *)&m2);
  // Histogram done
    Momt3d((s32 *)&wt, (s32 *)&mr, (s32 *)&mg, (s32 *)&mb, (float *)&m2);
  // Moments done

    cube[0].r0 = cube[0].g0 = cube[0].b0 = 0;
    cube[0].r1 = cube[0].g1 = cube[0].b1 = 32;
    next = 0;

    for (i = 1; i < PalSize; ++i)
    {
        if (Cut(&cube[next], &cube[i]))
        {
      // volume test ensures we won't try to cut one-cell box
            vv[next] = (float)((cube[next].vol > 1) ? Var(&cube[next]) : 0.0);
            vv[i] = (float)((cube[i].vol > 1) ? Var(&cube[i]) : 0.0);
        }
        else
        {
            vv[next] = 0.0;
            i--;
        }

        next = 0;
        temp = vv[0];

        for (k = 1; k <= i; ++k)
        {
            if (vv[k] > temp)
            {
                temp = vv[k];
                next = k;
            }
        }

        if (temp <= 0.0)
        {
            PalSize = i + 1;
            break;
        }
    }

    tag = Atag;

    for (k = 0; k < PalSize; ++k)
    {
        Mark(&cube[k], k, tag);
        weight = Vol(&cube[k], wt);

        if (weight)
        {

            QuantizedPalette[k][2] = (unsigned char)(Vol(&cube[k], mr) / weight);
            QuantizedPalette[k][0] = (unsigned char)(Vol(&cube[k], mb) / weight);
            QuantizedPalette[k][1] = (unsigned char)(Vol(&cube[k], mg) / weight);
        }
        else
        {
            QuantizedPalette[k][0] = QuantizedPalette[k][1] = QuantizedPalette[k][2] = 0;
        }
    }

    for (i = 0; i < ImageSize; i++)
        Picture256[i] = tag[Qadd[i]];

    return 0;
}
