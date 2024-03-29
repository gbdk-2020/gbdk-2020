;-------------------------------------------------------------------------
;   _modulong.s - routine for remainder of 32 bit unsigned long division
;
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

	.module _modulong
	
;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __modulong
	
;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res0 "__modulong_PARM_1+0"
	.define res1 "__modulong_PARM_1+1"
	.define res2 "___SDCC_m6502_ret2"
	.define res3 "___SDCC_m6502_ret3"
	.define den  "__modulong_PARM_2"
	.define rem  "___SDCC_m6502_ret4"
	.define s1   "___SDCC_m6502_ret0"
	.define s2   "___SDCC_m6502_ret1"
	
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE

__modulong:
	jsr 	___udivmod32
	lda 	*rem+3
	sta	*res3
	lda	*rem+2
	sta	*res2
	ldx	*rem+1
	lda	*rem+0
	rts
