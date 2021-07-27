;--------------------------------------------------------------------------
;  mulchar.s
;
;  Copyright (C) 2000, Michael Hope
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

.area   _CODE

.globl	__mulint

__mulint:
        pop     af
        pop     bc
        pop     de
        push    de
        push    bc
        push    af

	;; 16-bit multiplication
	;;
	;; Entry conditions
	;; bc = multiplicand
	;; de = multiplier
	;;
	;; Exit conditions
	;; hl = less significant word of product
	;;
	;; Register used: AF,BC,DE,HL
__mul16::
	xor	a,a
	ld	l,a
	or	a,b
	ld	b,#16

        ;; Optimise for the case when this side has 8 bits of data or
        ;; less.  This is often the case with support address calls.
        jr      NZ,2$
        ld      b,#8
        ld      a,c
1$:
        ;; Taken from z88dk, which originally borrowed from the
        ;; Spectrum rom.
        add     hl,hl
2$:
        rl      c
        rla                     ;DLE 27/11/98
        jr      NC,3$
        add     hl,de
3$:
        djnz    1$
        ret

