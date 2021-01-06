/*-------------------------------------------------------------------------
   _modslong.c - routine for modulus of 32 bit signed long

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net

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

#ifndef GBDK
#include <sdcc-lib.h>
#endif

#if _SDCC_MANGLES_SUPPORT_FUNS || GBDK
unsigned long _modulong (unsigned long a, unsigned long b);
#endif

/*   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#if !defined(SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(SDCC_mcs51)
#    if defined(SDCC_MODEL_SMALL)
#      if defined(SDCC_STACK_AUTO)
#        define _MODSLONG_ASM_SMALL_AUTO
#      else
#        define _MODSLONG_ASM_SMALL
#      endif
#    endif
#  endif
#endif

#if defined _MODSLONG_ASM_SMALL

static void
_modslong_dummy (void) _naked
{
	_asm

		#define a0	dpl
		#define a1	dph
		#define a2	b
		#define a3	r1

		.globl __modslong

		// _modslong_PARM_2 shares the same memory with _modulong_PARM_2
		// and is defined in _modulong.c
		#define b0      (__modslong_PARM_2)
		#define b1      (__modslong_PARM_2 + 1)
		#define b2      (__modslong_PARM_2 + 2)
		#define b3      (__modslong_PARM_2 + 3)

	__modslong:
					; a3 in acc
					; b3 in (__modslong_PARM_2 + 3)
		mov	a3,a		; save a3

		clr	F0 		; Flag 0 in PSW
					; available to user for general purpose
		jnb	acc.7,a_not_negative

		setb	F0

		clr	a		; a = -a;
		clr	c
		subb	a,a0
		mov	a0,a
		clr	a
		subb	a,a1
		mov	a1,a
		clr	a
		subb	a,a2
		mov	a2,a
		clr	a
		subb	a,a3
		mov	a3,a

	a_not_negative:

		mov	a,b3
		jnb	acc.7,b_not_negative

		cpl	F0

		clr	a		; b = -b;
		clr	c
		subb	a,b0
		mov	b0,a
		clr	a
		subb	a,b1
		mov	b1,a
		clr	a
		subb	a,b2
		mov	b2,a
		clr	a
		subb	a,b3
		mov	b3,a

	b_not_negative:

		mov	a,a3		; restore a3 in acc

		lcall	__modulong

		jnb	F0,not_negative

				; result in (a == r1), b, dph, dpl
		clr	a
		clr	c
		subb	a,a0
		mov	a0,a
		clr	a
		subb	a,a1
		mov	a1,a
		clr	a
		subb	a,a2
		mov	a2,a
		clr	a
		subb	a,a3
				; result in a, b, dph, dpl
	not_negative:
		ret

	_endasm ;
}

#elif defined _MODSLONG_ASM_SMALL_AUTO

static void
_modslong_dummy (void) _naked
{
	_asm

		#define a0	dpl
		#define a1	dph
		#define a2	b
		#define a3	r1

		#define b0	r2
		#define b1	r3
		#define b2	r4
		#define b3	r5

		ar2 = 2			; BUG register set is not considered
		ar3 = 3
		ar4 = 4
		ar5 = 5

		.globl __modslong

	__modslong:

					; a3 in acc
		mov	a3,a		; save a3

		clr	F0		; F0 (Flag 0)
					; available to user for general purpose
		jnb	acc.7,a_not_negative

		setb	F0

		clr	a		; a = -a;
		clr	c
		subb	a,a0
		mov	a0,a
		clr	a
		subb	a,a1
		mov	a1,a
		clr	a
		subb	a,a2
		mov	a2,a
		clr	a
		subb	a,a3
		mov	a3,a

	a_not_negative:

		mov	a,sp
		add	a,#-2-3		; 2 bytes return address, 3 bytes param b
		mov	r0,a		; r1 points to b0


		mov	ar2,@r0		; load b0
		inc	r0		; r0 points to b1
		mov	ar3,@r0		; b1
		inc	r0
		mov	ar4,@r0		; b2
		inc	r0
		mov	a,@r0		; b3
		mov	b3,a

		jnb	acc.7,b_not_negative

		cpl	F0

		clr	a		; b = -b;
		clr	c
		subb	a,b0
		mov	b0,a
		clr	a
		subb	a,b1
		mov	b1,a
		clr	a
		subb	a,b2
		mov	b2,a
		clr	a
		subb	a,b3
		mov	b3,a

	b_not_negative:

		lcall	__modlong

		jnb	F0,not_negative

				; result in (a == r1), b, dph, dpl
		clr	a
		clr	c
		subb	a,a0
		mov	a0,a
		clr	a
		subb	a,a1
		mov	a1,a
		clr	a
		subb	a,a2
		mov	a2,a
		clr	a
		subb	a,a3	; result in a, b, dph, dpl

	not_negative:
		ret

	_endasm ;
}

#else // _MODSLONG_ASM

long
_modslong (long a, long b)
{
  long r;

  r = _modulong((a < 0 ? -a : a),
                (b < 0 ? -b : b));
  if ( (a < 0) ^ (b < 0))
    return -r;
  else
    return r;
}

#endif // _MODSLONG_ASM
