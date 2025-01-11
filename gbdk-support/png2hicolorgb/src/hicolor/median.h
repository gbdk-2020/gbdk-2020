#ifndef __median_h__
#define __median_h__

#include "defines.h"


#define MAXNUMCOLORS 256

#define PRECISION_R 	5
#define PRECISION_G 	5
#define PRECISION_B 	5

#define R_SCALE  	<< 1
#define G_SCALE  	* 3
#define B_SCALE

#define HIST_R_ELEMS 	(1<<PRECISION_R)
#define HIST_G_ELEMS 	(1<<PRECISION_G)
#define HIST_B_ELEMS 	(1<<PRECISION_B)

#define MR 		HIST_G_ELEMS*HIST_B_ELEMS
#define MG 		HIST_B_ELEMS

#define BITS_IN_SAMPLE 	8

#define R_SHIFT  	(BITS_IN_SAMPLE - PRECISION_R)
#define G_SHIFT  	(BITS_IN_SAMPLE - PRECISION_G)
#define B_SHIFT  	(BITS_IN_SAMPLE - PRECISION_B)

typedef struct _Color Color;
typedef struct _QuantizeObj QuantizeObj;
typedef void    (*Pass_Func) (QuantizeObj *, unsigned char *, unsigned char *, long, long);
typedef unsigned long ColorFreq;

typedef ColorFreq *Histogram;


struct _Color
{
	int		red;
    int		green;
    int		blue;
};

typedef struct
{
	int		Rmin, Rmax;		// The bounds of the box (inclusive); expressed as histogram indexes
	int		Gmin, Gmax;
    int		Bmin, Bmax;
    int		volume;			// The volume (actually 2-norm) of the box
    long	colorcount;		//The number of nonzero histogram cells within this box */
} mbox, *boxptr;





extern	u8	Picture256[160*BUF_HEIGHT*3];
extern	u8	QuantizedPalette[256][3];


void zero_histogram_rgb(void);
boxptr find_biggest_color_pop(boxptr boxlist,s32 numboxes);
boxptr find_biggest_volume(boxptr boxlist,s32 numboxes);
void update_box_rgb(boxptr boxp);
s32 median_cut_rgb(boxptr boxlist,s32 numboxes);
void compute_color_rgb(boxptr boxp,s32 icolor);
s32 find_nearby_colors(s32 minR,s32 minG,s32 minB,s32 colorlist[]);
void find_best_colors(s32 minR, s32 minG, s32 minB,s32 numcolors,s32 colorlist[],s32 bestcolor[]);
void fill_inverse_cmap_rgb(s32 R, s32 G, s32 B);
void median_cut_pass1_rgb(u8 *src,s32 width,s32 height);
s32 *init_error_limit(void);
void to_indexed(u8 *input,s32 dither,s32 width,s32 height);


#endif
