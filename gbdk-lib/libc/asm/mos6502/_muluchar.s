;-------------------------------------------------------------------------
;   _muluchar.s - routine for multiplication of 16 bit (unsigned) int
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

	.module _muluchar

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __muluchar
	.globl ___umul8
	
;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define arg1 "___SDCC_m6502_ret0"
	.define arg2 "___SDCC_m6502_ret2"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

__muluchar:
	sta     arg1
	stx	arg2
___umul8:
        lda     #0              ; Clear byte 1
        ldy     #8              ; Number of bits
        lsr     arg2            ; Get first bit of RHS into carry
L0:    	bcc	L1
        clc
        adc     arg1
L1:    	ror
        ror     arg2
        dey
        bne    	L0
        tax
;//        stx     arg2+1          ; Result in .XA and ptr1
        lda     arg2            ; Load the result
        rts                     ; Done
