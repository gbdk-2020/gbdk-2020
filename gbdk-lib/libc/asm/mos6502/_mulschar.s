;-------------------------------------------------------------------------
;   _mulschar.s - routine for multiplication of 16 bit (unsigned) int
;
;   Copyright (C) 2009, Ullrich von Bassewitz
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

	.module _mulschar

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __mulschar
	.globl __muluschar
	.globl __mulsuchar

;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define arg1 "___SDCC_m6502_ret0"
	.define arg2 "___SDCC_m6502_ret2"
	.define s1 "___SDCC_m6502_ret4"
	.define s2 "___SDCC_m6502_ret5"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

__mulschar:
	sta	s1
	cmp	#0x00
	bpl 	pos1
	sec
	eor	#0xff
	adc	#0x00
pos1:	
	sta     arg1
	txa
	sta	s2
	bpl 	pos2
	sec
	eor	#0xff
	adc	#0x00
pos2:	
	sta     arg2
	
	jsr 	___umul8
	lda	s1
	eor	s2
	bpl	skip
	lda	arg2
	jmp	___negax

skip:	lda 	arg2
	rts

__muluschar:
	stx	__mulint_PARM_2
	ldx	#0x00
	stx	__mulint_PARM_2+1
	cmp	#0x00
	bpl	pos1m
	ldx 	#0xff
pos1m:
	jmp	__mulint
	
__mulsuchar:
	sta	__mulint_PARM_2
	txa
	ldx	#0x00
	stx	__mulint_PARM_2+1
	cmp	#0x00
	bpl	pos2m
	ldx 	#0xff
pos2m:
	jmp	__mulint
