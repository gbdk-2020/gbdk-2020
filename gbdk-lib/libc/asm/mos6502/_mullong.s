;-------------------------------------------------------------------------
;   _mullong.s - routine for multiplication of 32 bit (unsigned) long
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

	.module _mullong

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __mullong_PARM_2
	.globl __mullong_PARM_1
	.globl __mullong

;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
__mullong_PARM_1:
	.ds 4
__mullong_PARM_2:
	.ds 4

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define tmp  "___SDCC_m6502_ret4"
	.define res0 "__mullong_PARM_1+0"
	.define res1 "__mullong_PARM_1+1"
	.define res2 "___SDCC_m6502_ret2"
	.define res3 "___SDCC_m6502_ret3"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

__mullong:

        ldx	*__mullong_PARM_1+3
        stx	*res3
        ldx	*__mullong_PARM_1+2
        stx	*res2
;        ldx	*__mullong_PARM_1+1
;        stx	*res1
;        ldx	*__mullong_PARM_1+0
;        stx	*res0

        lda     #0
        sta     *tmp+2
        sta     *tmp+1
        sta     *tmp+0

        ldy     #32
next_bit:
	lsr     *tmp+2
        ror     *tmp+1
        ror     *tmp+0
        ror     a
        ror     *res3
        ror     *res2
        ror     *res1
        ror     *res0
        bcc     skip
        clc
        adc     *__mullong_PARM_2+0
        tax
        lda     *__mullong_PARM_2+1
        adc     *tmp+0
        sta     *tmp+0
        lda     *__mullong_PARM_2+2
        adc     *tmp+1
        sta     *tmp+1
        lda     *__mullong_PARM_2+3
        adc     *tmp+2
        sta     *tmp+2
        txa
skip:
        dey
        bpl	next_bit
        ldx	*res1
        lda	*res0
	rts
