;-------------------------------------------------------------------------
;   _mullong.s - routine for multiplication of 32 bit (unsigned) long
;
;   Copyright (C) 2025, Gabriele Gorla
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
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
__mullong_PARM_1:
	.ds 4
__mullong_PARM_2:
	.ds 4

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res0 "___SDCC_m6502_ret0"
	.define res1 "___SDCC_m6502_ret1"
	.define res2 "___SDCC_m6502_ret2"
	.define res3 "___SDCC_m6502_ret3"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE

__mullong:
	ldx     #0
	stx     *res0
	stx     *res1
	stx     *res2
;	stx	*res3

	ldy     #8
loop0:
	lsr	*__mullong_PARM_1+0
	bcc	skip0
	clc
	lda	*res0
	adc	*__mullong_PARM_2+0
	sta	*res0
	lda	*res1
	adc	*__mullong_PARM_2+1
	sta	*res1
	lda	*res2
	adc	*__mullong_PARM_2+2
	sta	*res2
;	lda	*res3
	txa
	adc	*__mullong_PARM_2+3
	tax
;	sta	*res3
skip0:
	asl	*__mullong_PARM_2+0
	rol	*__mullong_PARM_2+1
	rol	*__mullong_PARM_2+2
	rol	*__mullong_PARM_2+3
	dey
	bne	loop0

        ldy     #8
loop1:
	lsr 	*__mullong_PARM_1+1
	bcc 	skip1
	clc
	lda 	*res1
	adc	*__mullong_PARM_2+1
	sta 	*res1
	lda 	*res2
	adc 	*__mullong_PARM_2+2
	sta 	*res2
;	lda 	*res3
	txa
	adc	*__mullong_PARM_2+3
;	sta 	*res3
	tax
skip1:
	asl	*__mullong_PARM_2+1
	rol	*__mullong_PARM_2+2
	rol	*__mullong_PARM_2+3
	dey
	bne	loop1
    
	ldy	#8
loop2:
	lsr	*__mullong_PARM_1+2
	bcc	skip2
	clc
	lda	*res2
	adc	*__mullong_PARM_2+2
	sta	*res2
	txa
	adc	*__mullong_PARM_2+3
	tax
skip2:
	asl	*__mullong_PARM_2+2
	rol	*__mullong_PARM_2+3
	dey
	bne loop2

	txa
loop3:
	lsr 	*__mullong_PARM_1+3
	bcc 	skip3
	clc
	adc 	*__mullong_PARM_2+3
skip3:
	asl 	*__mullong_PARM_2+3
	bne 	loop3

	sta 	*res3
  	ldx	*res1
  	lda	*res0
	rts
