;--------------------------------------------------------------------------
;  __itoa.s
;
;  Copyright (C) 2020, Sergey Belyashov
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

	.globl ___itoa
	.globl ___uitoa
;
;void __itoa(int value, char *string, unsigned char radix);
;
___itoa::
	push	ix
	ld	ix, #0
	add	ix, sp
;
;	4(ix) - value
;	6(ix) - string
;	8(ix) - radix
;
	ld	e, 4 (ix)
	ld	d, 5 (ix)
	bit	7, d
	jr	Z, ___uitoa_de
;positive/negative numbers are supported only for radix=10
	ld	a, 8 (ix)
	cp	a, #10
	jr	NZ, ___uitoa_de
;add minus sign to result and inverse value
	ld	hl, #0
	or	a, a
	sbc	hl, de
	ex	de, hl
	ld	l, 6 (ix)
	ld	h, 7 (ix)
	ld	(hl), #0x2D	;minus symbol
	inc	hl
	ld	6 (ix), l
	ld	7 (ix), h
	jr	___uitoa_dehl
;
;void __uitoa(unsigned int value, char *string, unsigned char radix);
;
___uitoa::
	push	ix
	ld	ix, #0
	add	ix, sp
;
;	4(ix) - value
;	6(ix) - string
;	8(ix) - radix
;
	ld	e, 4 (ix)
	ld	d, 5 (ix)
;
___uitoa_de:
	ld	l, 6 (ix)
	ld	h, 7 (ix)
;
___uitoa_dehl:
	ld	a, e
	or	a, d
	jr	NZ, 100$
;
	ld	(hl), #0x30
	inc	hl
	jp	190$
100$:
	ld	a, 8 (ix)
	cp	a, #10		;most popular radix
	jr	NZ, 110$
;
;-------- decimal convertion
; this algorithm up to 2 times faster than generic
;
	ld	c, l
	ld	b, h
	ld	hl, #-4
	add	hl, sp
	ld	sp, hl
	push	bc
	push	hl
	push	de
	call	___uitobcd
	ld	hl, #4
	add	hl, sp
	ld	sp, hl
	pop	de		;DE - pointer to string
	inc	hl
	inc	hl		;HL - pointer to BCD value
	ld	b, #3		;number of bytes in BCD value
	ld	a, #0x30	;ASCII code of '0'
103$:
	rrd
	ld	(de), a
	inc	de
	rrd
	ld	(de), a
	inc	de
	inc	hl
	djnz	103$
;
;	pop	af
;	pop	af
;skip trailing zeroes
	ld	b, #5		;real BCD number is at most 5 digits
	dec	de		;so always skip last zero
105$:
	dec	de
	ld	a, (de)
	cp	a, #0x30
	jr	NZ, 107$	;break loop if non-zero found
	djnz	105$
107$:
	inc	de		;always point to symbol next to last significant
	ex	de, hl
	jr	190$
;
;---------------------------
;
110$:
	cp	a, #2
	jr	C, 190$		;radix is less than 2
;
	ld	c, a
	dec	c
	and	a, c
	jr	NZ, 150$
;
;-------- radix is power of 2
;
; DE - value, HL - pointer to string, C - mask
120$:
	ld	a, e
	ld	b, c
125$:
	srl	d
	rr	e
	srl	b
	jr	NZ, 125$
;
	and	a, c
	add	a, #0x30
	cp	a, #0x3A ;convert to 0...9A...Z
	jr	C, 130$
	add	a, #7
130$:
	ld	(hl), a
	inc	hl
	ld	a, e
	or	a, d
	jr	NZ, 120$
	jr	190$
;
;---------------------------
;
;-------- custom radix (generic algorithm)
;
150$:
	ex	de, hl
160$:
	ld	c, 8 (ix)
	call	___divu16_8
	add	a, #0x30
	cp	a, #0x3A
	jr	C, 165$
	add	a, #7
165$:
	ld	(de), a
	inc	de
	ld	a, l
	or	h
	jr	NZ, 160$
	ex	de, hl
;	jr	190$
;
;---------------------------
;
;-------- finish string and reverse order
190$:
	ld	(hl), #0
	ld	e, 6 (ix)
	ld	d, 7 (ix)
	call	___strreverse_reg
	ld	sp, ix
	pop	ix
	ret
;
;
;in: HL - divident, C - divisor
;out: HL - quotient, A - remainder
___divu16_8:
	xor	a, a
	ld	b, #16
100$:
	add	hl, hl
	rla
	jr	c, 110$
	cp	a, c
	jr	c, 120$
110$:
	sub	a, c
	inc	l
120$:
	djnz	100$
	ret

