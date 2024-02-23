;-------------------------------------------------------------------------
;   memcmp.s - standarc C library
;
;   Copyright (C) 2003, Ullrich von Bassewitz
;   Copyright (C) 2009, Christian Krueger
;   Copyright (C) 2023, Gabriele Gorla
;
;   This library is free software; you can redistribute it and/or modify it
;   under the terms of the GNU General Public License as published by the
;   Free Software Foundation; either version 2, or (at your option) any
;   later version.
;
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this library; see the file COPYING. If not, write to the
;   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;   As a special exception, if you link this library with other files,
;   some of which are compiled with SDCC, to produce an executable,
;   this library does not by itself cause the resulting executable to
;   be covered by the GNU General Public License. This exception does
;   not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;-------------------------------------------------------------------------

	.module memcmp

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl _memcmp_PARM_2
	.globl _memcmp_PARM_3
	.globl _memcmp

;--------------------------------------------------------
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
_memcmp_PARM_2:
	.ds 2
_memcmp_PARM_3:
	.ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define s1    "DPTR"
	.define s2    "_memcmp_PARM_2"
	.define count "_memcmp_PARM_3"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
	
;--------------------------------------------------------
; int memcmp (int *s1, int *s2, int count)
;--------------------------------------------------------

_memcmp:
	sta	*s1+0
	stx	*s1+1
	ldy	#0
	ldx	*count+1
	beq	endhi
hiloop:
	lda	[s1],y
	cmp	[s2],y
	bne	noteq
	iny
	bne	hiloop
	inc	*s1+1
	inc	*s2+1
	dex
	bne	hiloop
endhi:
	ldx	*count+0
	beq	end
loloop:
	lda	[s1],y
	cmp	[s2],y
	bne	noteq
	iny
	dex
	bne	loloop
end:
	txa
	rts
noteq:
	bcs	L2
	ldx	#0xFF
	rts
L2:
	ldx	#0x01
	rts
