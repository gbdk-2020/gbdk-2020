;-------------------------------------------------------------------------
;   _moduchar.s - routine for 8 bit (unsigned char) modulo
;
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

	.module _moduchar

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __moduchar   ; arguments in A and X, result in AX
	
;--------------------------------------------------------
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
__moduchar:
	stx	*__moduint_PARM_2
	ldx	#0x00
	stx	*(__moduint_PARM_2 + 1)
	jmp	__moduint

