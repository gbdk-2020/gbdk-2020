;-------------------------------------------------------------------------
;   memcpy.s - standarc C library
;
;   Copyright (C) 2003, Ullrich von Bassewitz
;   Copyright (C) 2009, Christian Krueger
;   Copyright (C) 2022, Gabriele Gorla
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

	.module ___memcpy

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___memcpy_PARM_2
	.globl ___memcpy_PARM_3
	.globl ___memcpy

;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
___memcpy_PARM_2:
	.ds 2
___memcpy_PARM_3:
	.ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define save  "___SDCC_m6502_ret0"
	.define dst   "___SDCC_m6502_ret2"
	.define src   "___memcpy_PARM_2"
	.define count "___memcpy_PARM_3"
	
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___memcpy:
	sta	*save+0
	stx	*save+1
	sta	*dst+0
	stx	*dst+1

	ldy	#0
	ldx	*count+1
	beq	L2
L1:
	lda	[*src],y
	sta	[*dst],y
	iny
	lda	[*src],y
	sta	[*dst],y
	iny
	bne	L1
	inc	*src+1
	inc	*dst+1
	dex
	bne	L1
L2:
	ldx	*count+0
	beq	done
L3:
	lda	[*src],y
	sta	[*dst],y
	iny
	dex
	bne	L3
done:
	lda	*save+0
	ldx	*save+1
	rts
