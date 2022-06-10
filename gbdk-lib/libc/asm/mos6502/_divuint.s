;-------------------------------------------------------------------------
;   _divuint.s - routine for division of 16 bit unsigned int
;
;   Copyright (C) 1998, Ullrich von Bassewitz
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

	.module _divuint

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __divuint_PARM_2
	.globl __divsint_PARM_2
	.globl __moduint_PARM_2
	.globl __modsint_PARM_2
	.globl __divuint
	.globl ___udivmod16

;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
__divuint_PARM_2:
__divsint_PARM_2:
__moduint_PARM_2:
__modsint_PARM_2:
	.ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res "___SDCC_m6502_ret0"
	.define den "__divuint_PARM_2"
	.define rem "___SDCC_m6502_ret2"
	.define s1  "___SDCC_m6502_ret4"
	.define s2  "___SDCC_m6502_ret5"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

__divuint:
	jsr	___udivmod16
	lda	*res+0
	ldx	*res+1
	rts

___udivmod16:
	sta	*res+0
	stx	*res+1

	lda	#0
	sta	*rem+1
	ldy	#16
;	ldx	__divuint_PARM_2+1
;	beq	div16x8
next_bit:
	asl	*res+0
	rol	*res+1
	rol	a
	rol	*rem+1

	tax
	cmp	*den+0
	lda	*rem+1
	sbc	*den+1
	bcc	L1
	sta	*rem+1
	txa
	sbc	*den+0
	tax
	inc	*res+0
L1:
	txa
	dey
	bne	next_bit
	sta	*rem+0
	rts

;div16x8:
;LL0:
;	asl	*___SDCC_m6502_ret0+0
;	rol	*___SDCC_m6502_ret0+1
;	rol	a
;	bcs	LL1
;	cmp	__divuint_PARM_2+0
;	bcc	LL2
;LL1:
;	sbc	__divuint_PARM_2+0
;	inc	*___SDCC_m6502_ret0+0
;LL2:
;	dey
;	bne	LL0
;	sta	*___SDCC_m6502_ret2+0
;
;	lda	*___SDCC_m6502_ret0+0
;	ldx	*___SDCC_m6502_ret0+1
;	rts

