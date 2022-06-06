;-------------------------------------------------------------------------
;   _divslong.s - routine for 32 bit signed long division
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

	.module _divslong

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __divslong
	.globl ___sdivmod32

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res0 "__divslong_PARM_1+0"
	.define res1 "__divslong_PARM_1+1"
	.define res2 "___SDCC_m6502_ret2"
	.define res3 "___SDCC_m6502_ret3"
	.define den  "__divslong_PARM_2"
	.define rem  "___SDCC_m6502_ret4"
	.define s1   "___SDCC_m6502_ret0"
	.define s2   "___SDCC_m6502_ret1"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE
__divslong:
	jsr	___sdivmod32
	lda	*s1
	eor	*s2
	bpl	pos
; neg res
	sec
	lda	#0x00
	sbc	*res0
	tay
	lda	#0x00
	sbc	*res1
	tax
	lda	#0x00
	sbc	*res2
	sta	*res2
	lda	#0x00
	sbc	*res3
	sta	*res3
	tya
	rts
pos:
	lda	*res0
	ldx	*res1
	rts

___sdivmod32:
	lda	*__divslong_PARM_1+3
	sta	*s1
	bpl 	pos1
	ldy	#0
	jsr	___neg_div32_param
pos1:
	lda	*__divslong_PARM_2+3
	sta	*s2
	bpl 	pos2
	ldy	#4
	jsr	___neg_div32_param
pos2:
	jmp 	___udivmod32

___neg_div32_param:
	sec
	ldx	#0x04
loop:
	lda	#0x00
	sbc	*__divslong_PARM_1+0,y
	sta	*__divslong_PARM_1+0,y
	iny
	dex
	bne loop
	rts

