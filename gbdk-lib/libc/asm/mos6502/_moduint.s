;-------------------------------------------------------------------------
;   _moduint.s - routine for remainder of 16 bit unsigned division
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

	.module _moduint

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __moduint
	
;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res "___SDCC_m6502_ret0"
	.define div "__moduint_PARM_2"
	.define rem "___SDCC_m6502_ret2"
	.define s1  "___SDCC_m6502_ret4"
	.define s2  "___SDCC_m6502_ret5"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE
	
__moduint:
	jsr 	___udivmod16
	lda	*rem+0
	ldx	*rem+1
	rts	
