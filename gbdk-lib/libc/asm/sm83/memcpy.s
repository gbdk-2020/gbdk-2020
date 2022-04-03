;--------------------------------------------------------------------------
;  memcpy.s
;
;  Copies in groups of four. Algorithm is Duff's device.
;
;  Copyright (c) 2021, Philipp Klaus Krause
;  Copyright (c) 2022, Sebastian 'basxto' Riedel <sdcc@basxto.de>
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

	.globl	_memcpy
	.globl	___memcpy

	; gets called by memmove for (s >= d)
	; bigger than the old algorithm but faster for n>=5
_memcpy::
___memcpy::
	;dest in de
	;src in bc
	;n in sp+2,sp+3
	push	de
	ldhl	sp, #5
	ld	a, (hl-)
	ld	l, (hl)
	ld	h, b
	ld	b, a
	ld	a, l
	ld	l, c
	srl	b
	rra
	ld	c, a
	;dest in de (backup in sp+0,sp+1)
	;src in hl
	;n/2 in bc
	;LSB of bc in carry
	jr nc, skip_one
	ld	a, (hl+)
	ld	(de), a
	inc	de
skip_one:
	;n/2 in bc
	;shift second LSB to carry
	srl	b
	rr	c
	;n/4 in bc
	inc	b
	inc	c
	jr nc, test
	jr	copy_two
.irp	idx,copy_four,copy_two
	idx:
	.rept	2
		ld	a, (hl+)
		ld	(de), a
		inc	de
	.endm
.endm
test:
.irp	idx,c,b
	dec	idx
	jr	nz, copy_four
.endm

end:
	;restore dest
	pop	bc
	;get return address
	pop	hl
	;throw away n
	pop	af
	jp	(hl)