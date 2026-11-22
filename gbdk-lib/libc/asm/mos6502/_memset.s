;-------------------------------------------------------------------------
;   _memset.s - standarc C library
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

	.module _memset

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl _memset_PARM_2
	.globl _memset_PARM_3
	.globl _memset
	
;--------------------------------------------------------
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
_memset_PARM_2:
	.ds 1
_memset_PARM_3:
	.ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define save  "REGTEMP+0"
	.define dst   "DPTR"
	.define val   "_memset_PARM_2"
	.define count "_memset_PARM_3"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE

_memset:
	sta	*dst+0
	stx	*dst+1
	stx	*save

	ldy	#0
	lda	*val
	ldx	*count+1
	beq	last_bytes

page_loop:
	sta	[dst],y
	iny
;	sta	[dst],y
;	iny
	bne	page_loop
	inc	*dst+1
	dex
	bne	page_loop

last_bytes:
	ldx	*count+0
	beq	end
byte_loop:
	sta	[dst],y
	iny
	dex
	bne	byte_loop
end:
	lda	*dst+0
	ldx	*save
	rts
