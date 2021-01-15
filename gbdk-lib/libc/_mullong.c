/*-------------------------------------------------------------------------
   _mullong.c - routine for multiplication of 32 bit (unsigned) long

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   Copyright (C) 1999, Jean Louis VERN jlvern@writeme.com

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

/* Signed and unsigned multiplication are the same - as long as the output
   has the same precision as the input.

   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#if !defined(__SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(__SDCC_mcs51)
#    if defined(__SDCC_MODEL_SMALL)
#      if defined(__SDCC_STACK_AUTO) && !defined(__SDCC_PARMS_IN_BANK1)
#        define _MULLONG_ASM_SMALL_AUTO
#      else
#        define _MULLONG_ASM_SMALL
#      endif
#    elif defined(__SDCC_MODEL_LARGE)
#      if !defined(__SDCC_STACK_AUTO)
#        define _MULLONG_ASM_LARGE
#      endif
#    endif
#  endif
#endif

#if defined(_MULLONG_ASM_SMALL) || defined(_MULLONG_ASM_SMALL_AUTO)

void
_mullong_dummy (void) __naked
{
	__asm

__mullong:

	.globl __mullong

				; the result c will be stored in r4...r7
	#define c0 r4
	#define c1 r5
	#define c2 r6
	#define c3 r7

	#define a0 dpl
	#define a1 dph
	#define a2 r2
	#define a3 r3

	; c0  a0 * b0
	; c1  a1 * b0 + a0 * b1
	; c2  a2 * b0 + a1 * b1 + a0 * b2
	; c3  a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3

#if !defined(__SDCC_STACK_AUTO) || defined(__SDCC_PARMS_IN_BANK1)
#if defined(__SDCC_PARMS_IN_BANK1)
	#define b0  (b1_0)
	#define b1  (b1_1)
	#define b2  (b1_2)
	#define b3  (b1_3)
#else
#if defined(__SDCC_NOOVERLAY)
	.area DSEG    (DATA)
#else
	.area OSEG    (OVR,DATA)
#endif

__mullong_PARM_2:

	.globl __mullong_PARM_2

	.ds	4

	b0 =  __mullong_PARM_2
	b1 = (__mullong_PARM_2+1)
	b2 = (__mullong_PARM_2+2)
	b3 = (__mullong_PARM_2+3)

#endif
	.area CSEG    (CODE)

				; parameter a comes in a, b, dph, dpl
	mov	r2,b		; save parameter a
	mov	r3,a

				;	Byte 0
	mov	a,a0
	mov	b,b0
	mul	ab		; a0 * b0
	mov	c0,a
	mov	c1,b

				;	Byte 1
	mov	a,a1
	mov	b,b0
	mul	ab		; a1 * b0
	add	a,c1
	mov	c1,a
	clr	a
	addc	a,b
	mov	c2,a

	mov	a,a0
	mov	b,b1
	mul	ab		; a0 * b1
	add	a,c1
	mov	c1,a
	mov	a,b
	addc	a,c2
	mov	c2,a
	clr	a
	rlc	a
	mov	c3,a

				;	Byte 2
	mov	a,a2
	mov	b,b0
	mul	ab		; a2 * b0
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,b1
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a0
	mov	b,b2
	mul	ab		; a0 * b2
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

				;	Byte 3
	mov	a,a3
	mov	b,b0
	mul	ab		; a3 * b0
	add	a,c3
	mov	c3,a

	mov	a,a2
	mov	b,b1
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,b2
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	a,a0
	mov	b,b3
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0
	ret

#else // __SDCC_STACK_AUTO

				; parameter a comes in a, b, dph, dpl
	mov	r2,b		; save parameter a
	mov	r3,a

	#define a0 dpl
	#define a1 dph
	#define a2 r2
	#define a3 r3

	#define b0 r1

	mov	a,#-2-3		;  1  return address 2 bytes, b 4 bytes
	add	a,sp		;  1
	mov	r0,a		;  1  r0 points to b0

				;	Byte 0
	mov	a,a0
	mov	b,@r0		; b0
	mov	b0,b		; we need b0 several times
	inc	r0		; r0 points to b1
	mul	ab		; a0 * b0
	mov	c0,a
	mov	c1,b

				;	Byte 1
	mov	a,a1
	mov	b,b0
	mul	ab		; a1 * b0
	add	a,c1
	mov	c1,a
	clr	a
	addc	a,b
	mov	c2,a

	mov	a,a0
	mov	b,@r0		; b1
	mul	ab		; a0 * b1
	add	a,c1
	mov	c1,a
	mov	a,b
	addc	a,c2
	mov	c2,a
	clr	a
	rlc	a
	mov	c3,a

				;	Byte 2
	mov	a,a2
	mov	b,b0
	mul	ab		; a2 * b0
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,@r0		; b1
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a0
	inc	r0
	mov	b,@r0		; b2
	mul	ab		; a0 * b2
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

				;	Byte 3
	mov	a,a3
	mov	b,b0
	mul	ab		; a3 * b0
	add	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,@r0		; b2
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	a,a2
	dec	r0
	mov	b,@r0		; b1
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	a,a0
	inc	r0
	inc	r0
	mov	b,@r0		; b3
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0

	ret

#endif // __SDCC_STACK_AUTO

	__endasm;
}

#elif defined(_MULLONG_ASM_LARGE)

void
_mullong_dummy (void) __naked
{
	__asm

__mullong:

	.globl __mullong

				; the result c will be stored in r4...r7
	#define c0 r4
	#define c1 r5
	#define c2 r6
	#define c3 r7

	; c0  a0 * b0
	; c1  a1 * b0 + a0 * b1
	; c2  a2 * b0 + a1 * b1 + a0 * b2
	; c3  a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3

#if !defined(__SDCC_PARMS_IN_BANK1)
	.area XSEG    (XDATA)

__mullong_PARM_2:

	.globl __mullong_PARM_2

	.ds	4
#endif
	.area CSEG    (CODE)

				; parameter a comes in a, b, dph, dpl
	mov	r0,dpl		; save parameter a
	mov	r1,dph
	mov	r2,b
	mov	r3,a

	#define a0 r0
	#define a1 r1
	#define a2 r2
	#define a3 r3

				;	Byte 0
	mov	b,a0
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_0		; b0
#else
	mov	dptr,#__mullong_PARM_2
	movx	a,@dptr		; b0
#endif
	mul	ab		; a0 * b0
	mov	c0,a
	mov	c1,b

				;	Byte 1
	mov	b,a1
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_0		; b0
#else
	movx	a,@dptr		; b0
#endif
	mul	ab		; a1 * b0
	add	a,c1
	mov	c1,a
	clr	a
	addc	a,b
	mov	c2,a

	mov	b,a0
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_1		; b1
#else
	inc	dptr		; b1
	movx	a,@dptr
#endif
	mul	ab		; a0 * b1
	add	a,c1
	mov	c1,a
	mov	a,b
	addc	a,c2
	mov	c2,a
	clr	a
	rlc	a
	mov	c3,a

				;	Byte 2
	mov	b,a1
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_1		; b1
#else
	movx	a,@dptr		; b1
#endif
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	b,a0
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_2		; b2
#else
	inc	dptr		; b2
	movx	a,@dptr
#endif
	mul	ab		; a0 * b2
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	b,a2
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_0		; b0
#else
	mov	dptr,#__mullong_PARM_2
	movx	a,@dptr		; b0
#endif
	mul	ab		; a2 * b0
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

				;	Byte 3
	mov	b,a3
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_0		; b0
#else
	movx	a,@dptr		; b0
#endif
	mul	ab		; a3 * b0
	add	a,c3
	mov	c3,a

	mov	b,a2
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_1		; b1
#else
	inc	dptr		; b1
	movx	a,@dptr
#endif
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	b,a1
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_2		; b2
#else
	inc	dptr		; b2
	movx	a,@dptr
#endif
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	b,a0
#if defined(__SDCC_PARMS_IN_BANK1)
	mov	a,b1_3		; b3
#else
	inc	dptr		; b3
	movx	a,@dptr
#endif
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0
	ret

	__endasm;
}

#elif defined(__SDCC_USE_XSTACK) && defined(__SDCC_STACK_AUTO)

void
_mullong_dummy (void) __naked
{
	__asm

__mullong:

	.globl __mullong

				; the result c will be stored in r4...r7
	#define c0 r4
	#define c1 r5
	#define c2 r6
	#define c3 r7

	#define a0 dpl
	#define a1 dph
	#define a2 r2
	#define a3 r3

	#define b0 r1

	; c0  a0 * b0
	; c1  a1 * b0 + a0 * b1
	; c2  a2 * b0 + a1 * b1 + a0 * b2
	; c3  a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3

				; parameter a comes in a, b, dph, dpl
	mov	r2,b		; save parameter a
	mov	r3,a

	mov	a,#-4		;  1  b 4 bytes
	add	a,_spx		;  1
	mov	r0,a		;  1  r0 points to b0

				;	Byte 0
	movx	a,@r0		; b0
	mov	b0,a		; we need b0 several times
	inc	r0		; r0 points to b1
	mov	b,a0
	mul	ab		; a0 * b0
	mov	c0,a
	mov	c1,b

				;	Byte 1
	mov	a,a1
	mov	b,b0
	mul	ab		; a1 * b0
	add	a,c1
	mov	c1,a
	clr	a
	addc	a,b
	mov	c2,a

	mov	b,a0
	movx	a,@r0		; b1
	mul	ab		; a0 * b1
	add	a,c1
	mov	c1,a
	mov	a,b
	addc	a,c2
	mov	c2,a
	clr	a
	rlc	a
	mov	c3,a

				;	Byte 2
	mov	a,a2
	mov	b,b0
	mul	ab		; a2 * b0
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	b,a1
	movx	a,@r0		; b1
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	b,a0
	inc	r0
	movx	a,@r0		; b2
	mul	ab		; a0 * b2
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

				;	Byte 3
	mov	a,a3
	mov	b,b0
	mul	ab		; a3 * b0
	add	a,c3
	mov	c3,a

	mov	b,a1
	movx	a,@r0		; b2
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	b,a2
	dec	r0
	movx	a,@r0		; b1
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	b,a0
	inc	r0
	inc	r0
	movx	a,@r0		; b3
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0

	ret

	__endasm;
}

#else // _MULLONG_ASM

struct some_struct {
	short a ;
	char b;
	long c ;};
#if defined(__SDCC_hc08) || defined(__SDCC_s08) || defined(__SDCC_stm8)
/* big endian order */
union bil {
        struct {unsigned char b3,b2,b1,b0 ;} b;
        struct {unsigned short hi,lo ;} i;
        unsigned long l;
        struct { unsigned char b3; unsigned short i12; unsigned char b0;} bi;
} ;
#else
/* little endian order */
union bil {
        struct {unsigned char b0,b1,b2,b3 ;} b;
        struct {unsigned short lo,hi ;} i;
        unsigned long l;
        struct { unsigned char b0; unsigned short i12; unsigned char b3;} bi;
} ;
#endif

#if defined(__SDCC)
 #include <sdcc-lib.h>
#endif

#define bcast(x) ((union bil _AUTOMEM *)&(x))

/*
                     3   2   1   0
       X             3   2   1   0
       ----------------------------
                   0.3 0.2 0.1 0.0
               1.3 1.2 1.1 1.0
           2.3 2.2 2.1 2.0
       3.3 3.2 3.1 3.0
       ----------------------------
                  |3.3|1.3|0.2|0.0|   A
                    |2.3|0.3|0.1|     B
                    |3.2|1.2|1.0|     C
                      |2.2|1.1|       D
                      |3.1|2.0|       E
                        |2.1|         F
                        |3.0|         G
                          |-------> only this side 32 x 32 -> 32
*/
#if defined(__SDCC_USE_XSTACK)
// currently the original code without u fails with --xstack
// it runs out of pointer registers
long
_mullong (long a, long b)
{
        union bil t, u;

        t.i.hi   = bcast(a)->b.b0 * bcast(b)->b.b2;          // A
        t.i.lo   = bcast(a)->b.b0 * bcast(b)->b.b0;          // A
        u.bi.b3  = bcast(a)->b.b0 * bcast(b)->b.b3;          // B
        u.bi.i12 = bcast(a)->b.b0 * bcast(b)->b.b1;          // B
        u.bi.b0  = 0;                                        // B
        t.l += u.l;

        t.b.b3  += bcast(a)->b.b3 * bcast(b)->b.b0;          // G
        t.b.b3  += bcast(a)->b.b2 * bcast(b)->b.b1;          // F
        t.i.hi  += bcast(a)->b.b2 * bcast(b)->b.b0;          // E
        t.i.hi  += bcast(a)->b.b1 * bcast(b)->b.b1;          // D

        u.bi.b3  = bcast(a)->b.b1 * bcast(b)->b.b2;          // C
        u.bi.i12 = bcast(a)->b.b1 * bcast(b)->b.b0;          // C
        u.bi.b0  = 0;                                        // C
        t.l += u.l;

        return t.l;
}
#elif defined(__SDCC_z80) || defined(__SDCC_gbz80) || defined(__SDCC_r2k) || defined(__SDCC_r3k)
/* 32x32->32 multiplication to be used
   if 16x16->16 is faster than three 8x8->16.
   2009, by M.Bodrato ( http://bodrato.it/ )

   z80 and gbz80 don't have any hardware multiplication.
   r2k and r3k have 16x16 hardware multiplication.
 */
long
_mullong (long a, long b)
{
  unsigned short i12;

  bcast(a)->i.hi *= bcast(b)->i.lo;
  bcast(a)->i.hi += bcast(b)->i.hi * bcast(a)->i.lo;

  /* only (a->i.lo * b->i.lo) 16x16->32 to do. asm? */
  bcast(a)->i.hi += bcast(a)->b.b1 * bcast(b)->b.b1;

  i12 = bcast(b)->b.b0 * bcast(a)->b.b1;
  bcast(b)->bi.i12 = bcast(a)->b.b0 * bcast(b)->b.b1;

  /* add up the two partial result, store carry in b3 */
  bcast(b)->b.b3 = ((bcast(b)->bi.i12 += i12) < i12);

  bcast(a)->i.lo  = bcast(a)->b.b0 * bcast(b)->b.b0;

  bcast(b)->bi.b0 = 0;

  return a + b;
}
#else
long
_mullong (long a, long b)
{
        union bil t;

        t.i.hi = bcast(a)->b.b0 * bcast(b)->b.b2;           // A
        t.i.lo = bcast(a)->b.b0 * bcast(b)->b.b0;           // A
        t.b.b3 += bcast(a)->b.b3 * bcast(b)->b.b0;          // G
        t.b.b3 += bcast(a)->b.b2 * bcast(b)->b.b1;          // F
        t.i.hi += bcast(a)->b.b2 * bcast(b)->b.b0;          // E <- b lost in .lst
        // bcast(a)->i.hi is free !
        t.i.hi += bcast(a)->b.b1 * bcast(b)->b.b1;          // D <- b lost in .lst

        bcast(a)->bi.b3 = bcast(a)->b.b1 * bcast(b)->b.b2;  // C
        bcast(a)->bi.i12 = bcast(a)->b.b1 * bcast(b)->b.b0; // C

        bcast(b)->bi.b3 = bcast(a)->b.b0 * bcast(b)->b.b3;  // B
        bcast(b)->bi.i12 = bcast(a)->b.b0 * bcast(b)->b.b1; // B

        bcast(b)->bi.b0 = 0;                                // B
        bcast(a)->bi.b0 = 0;                                // C
        t.l += a;

        return t.l + b;
}
#endif

#endif // _MULLONG_ASM
