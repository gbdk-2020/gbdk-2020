
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "fsdither.h" // tables:
#include "median.h"

Color		cmap[256];					//  colormap created by quantization
int			actual_number_of_colors;	//  Number of colors actually needed
Histogram	histogram;					//  holds the histogram
ColorFreq	AHistorgram[32768];
s32			Ared_n_row[60];
s32			Ared_p_row[60];
s32			Agrn_n_row[60];
s32			Agrn_p_row[60];
s32			Ablu_n_row[60];
s32			Ablu_p_row[60];
mbox			Abox[4];
s32			Atable[514];
u8			Picture256[160*BUF_HEIGHT*3];
u8			QuantizedPalette[256][3];




void zero_histogram_rgb(void)
{
	s32		r, g, b;

    for (r = 0; r < HIST_R_ELEMS; r++)
		for (g = 0; g < HIST_G_ELEMS; g++)
			for (b = 0; b < HIST_B_ELEMS; b++)
				histogram[r * MR + g * MG + b] = 0;
}



// Find the splittable box with the largest color population
// Returns 0 if no splittable boxes remain

boxptr find_biggest_color_pop(boxptr boxlist,s32 numboxes)
{
    boxptr	boxp;
    s32		i;
    s32		maxc = 0;
    boxptr	which = 0;

    for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++)
    {
		if (boxp->colorcount > maxc && boxp->volume > 0)
		{
		    which = boxp;
		    maxc = boxp->colorcount;
		}
    }

    return which;
}



// Find the splittable box with the largest (scaled) volume
// Returns 0 if no splittable boxes remain

boxptr find_biggest_volume(boxptr boxlist,s32 numboxes)
{
    boxptr	boxp;
    s32		i;
    s32		maxv = 0;
    boxptr	which = 0;

    for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++)
    {
		if (boxp->volume > maxv)
		{
		    which = boxp;
		    maxv = boxp->volume;
		}
    }

    return which;
}


// Shrink the min/max bounds of a box to enclose only nonzero elements,
// and recompute its volume and population

void update_box_rgb(boxptr boxp)
{
    ColorFreq	*histp;
    s32			R, G, B;
    s32			Rmin, Rmax, Gmin, Gmax, Bmin, Bmax;
    s32			dist0, dist1, dist2;
    s32			ccount;

    Rmin = boxp->Rmin;
    Rmax = boxp->Rmax;
    Gmin = boxp->Gmin;
    Gmax = boxp->Gmax;
    Bmin = boxp->Bmin;
    Bmax = boxp->Bmax;

    if (Rmax > Rmin)
		for (R = Rmin; R <= Rmax; R++)
		    for (G = Gmin; G <= Gmax; G++)
		    {
				histp = histogram + R * MR + G * MG + Bmin;
				for (B = Bmin; B <= Bmax; B++)
				    if (*histp++ != 0)
				    {
						boxp->Rmin = Rmin = R;
						goto have_Rmin;
				    }
		    }

  have_Rmin:

    if (Rmax > Rmin)
		for (R = Rmax; R >= Rmin; R--)
		    for (G = Gmin; G <= Gmax; G++)
		    {
				histp = histogram + R * MR + G * MG + Bmin;
				for (B = Bmin; B <= Bmax; B++)
				    if (*histp++ != 0)
				    {
						boxp->Rmax = Rmax = R;
						goto have_Rmax;
				    }
		    }

  have_Rmax:

    if (Gmax > Gmin)
		for (G = Gmin; G <= Gmax; G++)
		    for (R = Rmin; R <= Rmax; R++)
		    {
				histp = histogram + R * MR + G * MG + Bmin;
				for (B = Bmin; B <= Bmax; B++)
				    if (*histp++ != 0)
				    {
						boxp->Gmin = Gmin = G;
						goto have_Gmin;
				    }
		    }

  have_Gmin:

    if (Gmax > Gmin)
		for (G = Gmax; G >= Gmin; G--)
		    for (R = Rmin; R <= Rmax; R++)
		    {
				histp = histogram + R * MR + G * MG + Bmin;
				for (B = Bmin; B <= Bmax; B++)
				    if (*histp++ != 0)
				    {
						boxp->Gmax = Gmax = G;
						goto have_Gmax;
				    }
		    }

  have_Gmax:

    if (Bmax > Bmin)
		for (B = Bmin; B <= Bmax; B++)
		    for (R = Rmin; R <= Rmax; R++)
		    {
				histp = histogram + R * MR + Gmin * MG + B;
				for (G = Gmin; G <= Gmax; G++, histp += MG)
				    if (*histp != 0)
				    {
						boxp->Bmin = Bmin = B;
						goto have_Bmin;
				    }
		    }

  have_Bmin:

    if (Bmax > Bmin)
		for (B = Bmax; B >= Bmin; B--)
		    for (R = Rmin; R <= Rmax; R++)
		    {
				histp = histogram + R * MR + Gmin * MG + B;
				for (G = Gmin; G <= Gmax; G++, histp += MG)
				    if (*histp != 0)
				    {
						boxp->Bmax = Bmax = B;
						goto have_Bmax;
				    }
		    }

  have_Bmax:

    // Update box volume.
    // We use 2-norm rather than real volume here; this biases the method
    // against making long narrow boxes, and it has the side benefit that
    // a box is splittable iff norm > 0.
    // Since the differences are expressed in histogram-cell units,
    // we have to shift back to JSAMPLE units to get consistent distances;
    // after which, we scale according to the selected distance scale factors.

    dist0 = ((Rmax - Rmin) << R_SHIFT) R_SCALE;
    dist1 = ((Gmax - Gmin) << G_SHIFT) G_SCALE;
    dist2 = ((Bmax - Bmin) << B_SHIFT) B_SCALE;
    boxp->volume = dist0 * dist0 + dist1 * dist1 + dist2 * dist2;

    // Now scan remaining volume of box and compute population
    ccount = 0;
    for (R = Rmin; R <= Rmax; R++)
	{
		for (G = Gmin; G <= Gmax; G++)
		{
		    histp = histogram + R * MR + G * MG + Bmin;
		    for (B = Bmin; B <= Bmax; B++, histp++)
				if (*histp != 0)
				{
				    ccount++;
				}
		}
	}

    boxp->colorcount = ccount;
}


// Repeatedly select and split the largest box until we have enough boxes

s32 median_cut_rgb(boxptr boxlist,s32 numboxes)
{
    s32			n, lb;
    s32			R, G, B, cmax;
    boxptr		b1, b2;

    while (numboxes < 4)
    {
		// Select box to split. Current algorithm: by population for first half, then by volume.
		if (numboxes * 2 <= 4)
	    	b1 = find_biggest_color_pop(boxlist, numboxes);
		else
	    	b1 = find_biggest_volume(boxlist, numboxes);

		if (b1 == 0)		/* no splittable boxes left! */
		    break;

		b2 = boxlist + numboxes;	// where new box will go
		b2->Rmax = b1->Rmax;		// Copy the color bounds to the new box.
		b2->Gmax = b1->Gmax;
		b2->Bmax = b1->Bmax;
		b2->Rmin = b1->Rmin;
		b2->Gmin = b1->Gmin;
		b2->Bmin = b1->Bmin;

		// Choose which axis to split the box on Current algorithm: longest scaled axis.
		// See notes in update_box about scaling distances.

		R = ((b1->Rmax - b1->Rmin) << R_SHIFT) R_SCALE;
		G = ((b1->Gmax - b1->Gmin) << G_SHIFT) G_SCALE;
		B = ((b1->Bmax - b1->Bmin) << B_SHIFT) B_SCALE;

		// We want to break any ties in favor of green, then red, blue last.

		cmax = G;
		n = 1;

		if (R > cmax)
		{
	    	cmax = R;
		    n = 0;
		}

		if (B > cmax)
	    	n = 2;

		// Choose split point along selected axis, and update box bounds.
		// Current algorithm: split at halfway point.
		// (Since the box has been shrunk to minimum volume,
		// any split will produce two nonempty subboxes.)
		// Note that lb value is max for lower box, so must be < old max.

		switch (n)
		{
			case 0:

			    lb = (b1->Rmax + b1->Rmin) / 2;
			    b1->Rmax = lb;
			    b2->Rmin = lb + 1;
			    break;

			case 1:

			    lb = (b1->Gmax + b1->Gmin) / 2;
			    b1->Gmax = lb;
			    b2->Gmin = lb + 1;
			    break;

			case 2:

			    lb = (b1->Bmax + b1->Bmin) / 2;
			    b1->Bmax = lb;
			    b2->Bmin = lb + 1;
			    break;
		}

		// Update stats for boxes
		update_box_rgb(b1);
		update_box_rgb(b2);
		numboxes++;
    }

    return numboxes;
}

// Compute representative color for a box, put it in colormap[icolor]

void compute_color_rgb(boxptr boxp,s32 icolor)
{
    // Current algorithm: mean weighted by pixels (not colors)
    // Note it is important to get the rounding correct!
    ColorFreq	*histp;
    s32			R, G, B;
    s32			Rmin, Rmax;
    s32			Gmin, Gmax;
    s32			Bmin, Bmax;
    s32			count;
    s32			total = 0;
    s32			Rtotal = 0;
    s32			Gtotal = 0;
    s32			Btotal = 0;

    Rmin = boxp->Rmin;
    Rmax = boxp->Rmax;
    Gmin = boxp->Gmin;
    Gmax = boxp->Gmax;
    Bmin = boxp->Bmin;
    Bmax = boxp->Bmax;

    for (R = Rmin; R <= Rmax; R++)
	{
		for (G = Gmin; G <= Gmax; G++)
		{
		    histp = histogram + R * MR + G * MG + Bmin;
		    for (B = Bmin; B <= Bmax; B++)
		    {
				if ((count = *histp++) != 0)
				{
				    total += count;
				    Rtotal += ((R << R_SHIFT) + ((1 << R_SHIFT) >> 1)) * count;
				    Gtotal += ((G << G_SHIFT) + ((1 << G_SHIFT) >> 1)) * count;
				    Btotal += ((B << B_SHIFT) + ((1 << B_SHIFT) >> 1)) * count;
				}
			}
		}
	}

    cmap[icolor].red = (Rtotal + (total >> 1)) / total;
    cmap[icolor].green = (Gtotal + (total >> 1)) / total;
    cmap[icolor].blue = (Btotal + (total >> 1)) / total;
}


#define BOX_R_LOG  (PRECISION_R-3)
#define BOX_G_LOG  (PRECISION_G-3)
#define BOX_B_LOG  (PRECISION_B-3)

#define BOX_R_ELEMS  (1<<BOX_R_LOG)
#define BOX_G_ELEMS  (1<<BOX_G_LOG)
#define BOX_B_ELEMS  (1<<BOX_B_LOG)

#define BOX_R_SHIFT  (R_SHIFT + BOX_R_LOG)
#define BOX_G_SHIFT  (G_SHIFT + BOX_G_LOG)
#define BOX_B_SHIFT  (B_SHIFT + BOX_B_LOG)

/*
 * The next three routines implement inverse colormap filling.  They could
 * all be folded into one big routine, but splitting them up this way saves
 * some stack space (the mindist[] and bestdist[] arrays need not coexist)
 * and may allow some compilers to produce better code by registerizing more
 * inner-loop variables.
 */

s32 find_nearby_colors(s32 minR,s32 minG,s32 minB,s32 colorlist[])
/* Locate the colormap entries close enough to an update box to be candidates
 * for the nearest entry to some cell(s) in the update box.  The update box
 * is specified by the center coordinates of its first cell.  The number of
 * candidate colormap entries is returned, and their colormap indexes are
 * placed in colorlist[].
 * This routine uses Heckbert's "locally sorted search" criterion to select
 * the colors that need further consideration.
 */
{
    s32		numcolors = actual_number_of_colors;
    s32		maxR, maxG, maxB;
    s32		centerR, centerG, centerB;
    s32		i, x, ncolors;
    s32		minmaxdist, min_dist, max_dist, tdist;
    s32		mindist[MAXNUMCOLORS];	/* min distance to colormap entry i */

    /* Compute true coordinates of update box's upper corner and center.
     * Actually we compute the coordinates of the center of the upper-corner
     * histogram cell, which are the upper bounds of the volume we care about.
     * Note that since ">>" rounds down, the "center" values may be closer to
     * min than to max; hence comparisons to them must be "<=", not "<".
     */

    maxR = minR + ((1 << BOX_R_SHIFT) - (1 << R_SHIFT));
    centerR = (minR + maxR) >> 1;
    maxG = minG + ((1 << BOX_G_SHIFT) - (1 << G_SHIFT));
    centerG = (minG + maxG) >> 1;
    maxB = minB + ((1 << BOX_B_SHIFT) - (1 << B_SHIFT));
    centerB = (minB + maxB) >> 1;

    /* For each color in colormap, find:
     *  1. its minimum squared-distance to any point in the update box
     *     (zero if color is within update box);
     *  2. its maximum squared-distance to any point in the update box.
     * Both of these can be found by considering only the corners of the box.
     * We save the minimum distance for each color in mindist[];
     * only the smallest maximum distance is of interest.
     */

    minmaxdist = 0x7FFFFFFFL;

	for (i = 0; i < actual_number_of_colors; i++)
	{
		// We compute the squared-R-distance term, then add in the other two.
		x = cmap[i].red;

		if (x < minR)
		{
		    tdist = (x - minR) R_SCALE;
		    min_dist = tdist * tdist;
		    tdist = (x - maxR) R_SCALE;
		    max_dist = tdist * tdist;
		}
		else if (x > maxR)
		{
		    tdist = (x - maxR) R_SCALE;
		    min_dist = tdist * tdist;
		    tdist = (x - minR) R_SCALE;
		    max_dist = tdist * tdist;
		}
		else
		{
		    /* within cell range so no contribution to min_dist */
		    min_dist = 0;
		    if (x <= centerR)
		    {
				tdist = (x - maxR) R_SCALE;
				max_dist = tdist * tdist;
		    }
		    else
		    {
				tdist = (x - minR) R_SCALE;
				max_dist = tdist * tdist;
		    }
		}

		x = cmap[i].green;

		if (x < minG)
		{
		    tdist = (x - minG) G_SCALE;
		    min_dist += tdist * tdist;
		    tdist = (x - maxG) G_SCALE;
		    max_dist += tdist * tdist;
		}
		else if (x > maxG)
		{
		    tdist = (x - maxG) G_SCALE;
		    min_dist += tdist * tdist;
		    tdist = (x - minG) G_SCALE;
		    max_dist += tdist * tdist;
		}
		else
		{
		    // within cell range so no contribution to min_dist
		    if (x <= centerG)
		    {
				tdist = (x - maxG) G_SCALE;
				max_dist += tdist * tdist;
		    }
		    else
		    {
				tdist = (x - minG) G_SCALE;
				max_dist += tdist * tdist;
		    }
		}

		x = cmap[i].blue;

		if (x < minB)
		{
		    tdist = (x - minB) B_SCALE;
		    min_dist += tdist * tdist;
		    tdist = (x - maxB) B_SCALE;
		    max_dist += tdist * tdist;
		}
		else if (x > maxB)
		{
		    tdist = (x - maxB) B_SCALE;
		    min_dist += tdist * tdist;
		    tdist = (x - minB) B_SCALE;
		    max_dist += tdist * tdist;
		}
		else
		{
		    // within cell range so no contribution to min_dist
		    if (x <= centerB)
		    {
				tdist = (x - maxB) B_SCALE;
				max_dist += tdist * tdist;
		    }
		    else
		    {
				tdist = (x - minB) B_SCALE;
				max_dist += tdist * tdist;
		    }
		}

		mindist[i] = min_dist;	// save away the results
		if (max_dist < minmaxdist)
		    minmaxdist = max_dist;
    }

    // Now we know that no cell in the update box is more than minmaxdist
    // away from some colormap entry.  Therefore, only colors that are
    // within minmaxdist of some part of the box need be considered.

    ncolors = 0;

    for (i = 0; i < actual_number_of_colors; i++)
    {
		if (mindist[i] <= minmaxdist)
	    	colorlist[ncolors++] = i;
    }

    return ncolors;
}


/* Find the closest colormap entry for each cell in the update box,
  given the list of candidate colors prepared by find_nearby_colors.
  Return the indexes of the closest entries in the bestcolor[] array.
  This routine uses Thomas' incremental distance calculation method to
  find the distance from a colormap entry to successive cells in the box.
 */
void find_best_colors(s32 minR, s32 minG, s32 minB,s32 numcolors,s32 colorlist[],s32 bestcolor[])
{
    s32		iR, iG, iB;
    s32		i, icolor;
    s32		*bptr;	/* pointer into bestdist[] array */
    s32		*cptr;	/* pointer into bestcolor[] array */
    s32		dist0, dist1;	/* initial distance values */
    s32		dist2;	/* current distance in inner loop */
    s32		xx0, xx1;	/* distance increments */
    s32		xx2;
    s32		inR, inG, inB;	/* initial values for increments */

    // This array holds the distance to the nearest-so-far color for each cell */
    int             bestdist[BOX_R_ELEMS * BOX_G_ELEMS * BOX_B_ELEMS];

    // Initialize best-distance for each cell of the update box */
    bptr = bestdist;

    for (i = BOX_R_ELEMS * BOX_G_ELEMS * BOX_B_ELEMS - 1; i >= 0; i--)
		*bptr++ = 0x7FFFFFFFL;

    // For each color selected by find_nearby_colors,
    //  compute its distance to the center of each cell in the box.
    // If that's less than best-so-far, update best distance and color number.


    // Nominal steps between cell centers ("x" in Thomas article) */

#define STEP_R  ((1 << R_SHIFT) R_SCALE)
#define STEP_G  ((1 << G_SHIFT) G_SCALE)
#define STEP_B  ((1 << B_SHIFT) B_SCALE)

    for (i = 0; i < numcolors; i++)
    {
		icolor = colorlist[i];
		// Compute (square of) distance from minR/G/B to this color
		inR = (minR - cmap[icolor].red) R_SCALE;
		dist0 = inR * inR;
		inG = (minG - cmap[icolor].green) G_SCALE;
		dist0 += inG * inG;
		inB = (minB - cmap[icolor].blue) B_SCALE;
		dist0 += inB * inB;

		// Form the initial difference increments
		inR = inR * (2 * STEP_R) + STEP_R * STEP_R;
		inG = inG * (2 * STEP_G) + STEP_G * STEP_G;
		inB = inB * (2 * STEP_B) + STEP_B * STEP_B;

		// Now loop over all cells in box, updating distance per Thomas method
		bptr = bestdist;
		cptr = bestcolor;
		xx0 = inR;
		for (iR = BOX_R_ELEMS - 1; iR >= 0; iR--)
		{
		    dist1 = dist0;
		    xx1 = inG;
		    for (iG = BOX_G_ELEMS - 1; iG >= 0; iG--)
		    {
				dist2 = dist1;
				xx2 = inB;
				for (iB = BOX_B_ELEMS - 1; iB >= 0; iB--)
				{
				    if (dist2 < *bptr)
				    {
						*bptr = dist2;
						*cptr = icolor;
				    }

				    dist2 += xx2;
			    	xx2 += 2 * STEP_B * STEP_B;
				    bptr++;
				    cptr++;
				}

				dist1 += xx1;
				xx1 += 2 * STEP_G * STEP_G;
		    }
		    dist0 += xx0;
		    xx0 += 2 * STEP_R * STEP_R;
		}
    }
}


// Fill the inverse-colormap entries in the update box that contains
// histogram cell R/G/B.  (Only that one cell MUST be filled, but
// we can fill as many others as we wish.)

void fill_inverse_cmap_rgb(s32 R, s32 G, s32 B)
{
    s32			minR, minG, minB;	/* lower left corner of update box */
    s32			iR, iG, iB;
    s32			*cptr;	/* pointer into bestcolor[] array */
    ColorFreq	*cachep;	/* pointer into main cache array */
    /* This array lists the candidate colormap indexes. */
    s32			colorlist[MAXNUMCOLORS];
    s32			numcolors;	/* number of candidate colors */
    /* This array holds the actually closest colormap index for each cell. */
    s32			bestcolor[BOX_R_ELEMS * BOX_G_ELEMS * BOX_B_ELEMS];

    /* Convert cell coordinates to update box ID */
    R >>= BOX_R_LOG;
    G >>= BOX_G_LOG;
    B >>= BOX_B_LOG;

    /* Compute true coordinates of update box's origin corner.
     * Actually we compute the coordinates of the center of the corner
     * histogram cell, which are the lower bounds of the volume we care about.
     */

    minR = (R << BOX_R_SHIFT) + ((1 << R_SHIFT) >> 1);
    minG = (G << BOX_G_SHIFT) + ((1 << G_SHIFT) >> 1);
    minB = (B << BOX_B_SHIFT) + ((1 << B_SHIFT) >> 1);

    /* Determine which colormap entries are close enough to be candidates
     * for the nearest entry to some cell in the update box.
     */

    numcolors = find_nearby_colors(minR, minG, minB, colorlist);

    /* Determine the actually nearest colors. */
    find_best_colors(minR, minG, minB, numcolors, colorlist,bestcolor);

    /* Save the best color numbers (plus 1) in the main cache array */
    R <<= BOX_R_LOG;		/* convert ID back to base cell indexes */
    G <<= BOX_G_LOG;
    B <<= BOX_B_LOG;

    cptr = bestcolor;

    for (iR = 0; iR < BOX_R_ELEMS; iR++)
    {
		for (iG = 0; iG < BOX_G_ELEMS; iG++)
		{
	    	cachep = &histogram[(R + iR) * MR + (G + iG) * MG + B];

		    for (iB = 0; iB < BOX_B_ELEMS; iB++)
		    {
				*cachep++ = (*cptr++) + 1;
		    }
		}
    }
}




/*  This is pass 1  */
void median_cut_pass1_rgb(u8 *src,u8 *dest,s32 width,s32 height)
{
    s32             num_elems;
    ColorFreq      *col;
    boxptr          boxlist;
    s32             numboxes;
    s32             i;

    num_elems = width * height;
    zero_histogram_rgb();

    while (num_elems--)
    {
		col = &histogram[(src[0] >> R_SHIFT) * MR + (src[1] >> G_SHIFT) * MG + (src[2] >> B_SHIFT)];
		(*col)++;
		src += 3;
    }

    boxlist = (boxptr) Abox;

    // Initialize one box containing whole space
    numboxes = 1;
    boxlist[0].Rmin = 0;
    boxlist[0].Rmax = (1 << PRECISION_R) - 1;
    boxlist[0].Gmin = 0;
    boxlist[0].Gmax = (1 << PRECISION_G) - 1;
    boxlist[0].Bmin = 0;
    boxlist[0].Bmax = (1 << PRECISION_B) - 1;

    // Shrink it to actually-used volume and set its statistics
    update_box_rgb(boxlist);

    // Perform median-cut to produce final box list
    numboxes = median_cut_rgb(boxlist, numboxes);

    actual_number_of_colors = numboxes;

    // Compute the representative color for each box, fill colormap
    for (i = 0; i < numboxes; i++)
		compute_color_rgb(boxlist + i, i);
}


/* Map some rows of pixels to the output colormapped representation. */





/*
  Initialize the error-limiting transfer function (lookup table).
  The raw F-S error computation can potentially compute error values of up to
  +- MAXJSAMPLE.  But we want the maximum correction applied to a pixel to be
  much less, otherwise obviously wrong pixels will be created.  (Typical
  effects include weird fringes at color-area boundaries, isolated bright
  pixels in a dark area, etc.)  The standard advice for avoiding this problem
  is to ensure that the "corners" of the color cube are allocated as output
  colors; then repeated errors in the same direction cannot cause cascading
  error buildup.  However, that only prevents the error from getting
  completely out of hand; Aaron Giles reports that error limiting improves
  the results even with corner colors allocated.

  A simple clamping of the error values to about +- MAXJSAMPLE/8 works pretty
  well, but the smoother transfer function used below is even better.  Thanks
  to Aaron Giles for this idea.
 */

// Allocate and fill in the error_limiter table
s32 *init_error_limit(void)
{
    s32		*table;
    s32		in, out;

    table = Atable;
    table += 255;		// so we can index -255 ... +255

    // Map errors 1:1 up to +- 16
    out = 0;
    for (in = 0; in < 16; in++, out++)
	{
		table[in] = out;
		table[-in] = -out;
    }

    // Map errors 1:2 up to +- 3*16
    for (; in < 16 * 3; in++, out += (in & 1) ? 0 : 1)
	{
		table[in] = out;
		table[-in] = -out;
    }

    // Clamp the rest to final out value (which is 32)
    for (; in <= 255; in++)
	{
		table[in] = out;
		table[-in] = -out;
    }

    return table;
}



void to_indexed(u8 *input,s32 ncolors,s32 dither,s32 width,s32 height)
{
    s32				i, j;
	u8				*src;
	u8				*dest;
    s32				r, g, b;
    ColorFreq		*cachep;
    s32				row, col;

    Color			*color;
    u8				*src_row;
    u8				*dest_row;
    s32				*error_limiter;
    s16				*fs_err1, *fs_err2;
    s16				*fs_err3, *fs_err4;
    s32				*red_n_row, *red_p_row;
    s32				*grn_n_row, *grn_p_row;
    s32				*blu_n_row, *blu_p_row;
    s32				*rnr, *rpr;
    s32				*gnr, *gpr;
    s32				*bnr, *bpr;
    s32				*tmp;
    s32				re, ge, be;
    s32				index;
    s32				rowstride;
    s32				step_dest, step_src;
    s32				odd_row;
    u8				*range_limiter;

    histogram = AHistorgram;

    median_cut_pass1_rgb(input, Picture256, width, height);

	src=input;
	dest=Picture256;
    zero_histogram_rgb();

	if(dither)
	{
	    error_limiter = init_error_limit();
	    range_limiter = range_array + 256;

		red_n_row = Ared_n_row;
		red_p_row = Ared_p_row;
		grn_n_row = Agrn_n_row;
		grn_p_row = Agrn_p_row;
		blu_n_row = Ablu_n_row;
		blu_p_row = Ablu_p_row;

	    memset(red_p_row, 0, sizeof(int) * (width + 2));
	    memset(grn_p_row, 0, sizeof(int) * (width + 2));
	    memset(blu_p_row, 0, sizeof(int) * (width + 2));

	    fs_err1 = floyd_steinberg_error1 + 511;
	    fs_err2 = floyd_steinberg_error2 + 511;
	    fs_err3 = floyd_steinberg_error3 + 511;
	    fs_err4 = floyd_steinberg_error4 + 511;

	    src_row = src;
	    dest_row = dest;
	    rowstride = width * 3;
	    odd_row = 0;

	    for (row = 0; row < height; row++)
		{
			src = src_row;
			dest = dest_row;

			src_row += rowstride;
			dest_row += width;

			rnr = red_n_row;
			gnr = grn_n_row;
			bnr = blu_n_row;
			rpr = red_p_row + 1;
			gpr = grn_p_row + 1;
			bpr = blu_p_row + 1;

			if (odd_row)
			{
				step_dest = -1;
				step_src = -3;

				src += rowstride - 3;
				dest += width - 1;

				rnr += width + 1;
				gnr += width + 1;
				bnr += width + 1;
				rpr += width;
				gpr += width;
				bpr += width;

				*(rnr - 1) = *(gnr - 1) = *(bnr - 1) = 0;
			}
			else
			{
				step_dest = 1;
				step_src = 3;

				*(rnr + 1) = *(gnr + 1) = *(bnr + 1) = 0;
			}

			*rnr = *gnr = *bnr = 0;

			for (col = 0; col < width; col++)
			{
				r = range_limiter[src[0] + error_limiter[*rpr]];
				g = range_limiter[src[1] + error_limiter[*gpr]];
				b = range_limiter[src[2] + error_limiter[*bpr]];

				re = r >> R_SHIFT;
				ge = g >> G_SHIFT;
				be = b >> B_SHIFT;

				cachep = &histogram[re * MR + ge * MG + be];

				// If we have not seen this color before, find nearest colormap entry and update the cache
				if (*cachep == 0)
					fill_inverse_cmap_rgb(re, ge, be);

				index = *cachep - 1;
			    *dest = index;

			    color = &cmap[index];
			    re = r - color->red;
			    ge = g - color->green;
			    be = b - color->blue;

				if (odd_row)
				{
					*(--rpr) += fs_err1[re];
					*(--gpr) += fs_err1[ge];
					*(--bpr) += fs_err1[be];

					*rnr-- += fs_err2[re];
					*gnr-- += fs_err2[ge];
					*bnr-- += fs_err2[be];

					*rnr += fs_err3[re];
					*gnr += fs_err3[ge];
					*bnr += fs_err3[be];

					*(rnr - 1) = fs_err4[re];
					*(gnr - 1) = fs_err4[ge];
					*(bnr - 1) = fs_err4[be];
				}
				else
				{
					*(++rpr) += fs_err1[re];
					*(++gpr) += fs_err1[ge];
					*(++bpr) += fs_err1[be];

					*rnr++ += fs_err2[re];
					*gnr++ += fs_err2[ge];
					*bnr++ += fs_err2[be];

					*rnr += fs_err3[re];
					*gnr += fs_err3[ge];
					*bnr += fs_err3[be];

					*(rnr + 1) = fs_err4[re];
					*(gnr + 1) = fs_err4[ge];
					*(bnr + 1) = fs_err4[be];
				}

			    dest += step_dest;
			    src += step_src;
			}

			tmp = red_n_row;
			red_n_row = red_p_row;
			red_p_row = tmp;

			tmp = grn_n_row;
			grn_n_row = grn_p_row;
			grn_p_row = tmp;

			tmp = blu_n_row;
			blu_n_row = blu_p_row;
			blu_p_row = tmp;

			odd_row = !odd_row;
	    }

	}
	else
	{
	    for (row = 0; row < height; row++)
		{
			for (col = 0; col < width; col++)
			{
			    // get pixel value and index into the cache */
			    r = (*src++) >> R_SHIFT;
			    g = (*src++) >> G_SHIFT;
			    b = (*src++) >> B_SHIFT;

			    cachep = &histogram[r * MR + g * MG + b];

			    // If we have not seen this color before, find nearest colormap entry and update the cache
			    if (*cachep == 0)
					fill_inverse_cmap_rgb(r, g, b);

			    // Now emit the colormap index for this cell */
			    *dest++ = (unsigned char)(*cachep - 1);
			}

	    }
	}

    for (i = 0, j = 0; i < actual_number_of_colors; i++)
    {
		QuantizedPalette[j][0] = cmap[i].blue;
		QuantizedPalette[j][1] = cmap[i].green;
		QuantizedPalette[j++][2] = cmap[i].red;
    }
}

