;--------------------------------------------------------------------------
;  __uitobcd.s
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

	.globl ___uitobcd
;
; void __uitobcd (unsigned int v, unsigned char bcd[3])
; __uitobcd converts v to BCD representation to the bcd.
; bcd[] will contain BCD value.
;
___uitobcd:
	push	ix
	ld	ix, #0
	add	ix, sp
;
	ld	bc, #0x1000
	ld	d, c
	ld	e, c
	ld	l, 4 (ix)
	ld	h, 5 (ix)
;
;--- begin speed optimization
;
	ld	a, h
	or	a, a
	jr	NZ, 100$
;
	ld	h, l
	srl	b
;
;--- end speed optimization
;
; HL - binary value
; CDE - future BCD value
; B - bits count (16)
100$:
	add	hl, hl
	ld	a, e
	adc	a, a
	daa
	ld	e, a
	ld	a, d
	adc	a, a
	daa
	ld	d, a
	ld	a, c
	adc	a, a
	daa
	ld	c, a
	djnz	100$
;
	ld	l, 6 (ix)
	ld	h, 7 (ix)
	ld	(hl), e
	inc	hl
	ld	(hl), d
	inc	hl
	ld	(hl), c
;
	pop	ix
	ret
