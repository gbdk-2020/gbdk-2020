;-------------------------------------------------------------------------
;   _strcpy.s - standard C library function
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

	.module _strcpy

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl _strcpy_PARM_2
	.globl _strcpy

;--------------------------------------------------------
; overlayable function paramters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
_strcpy_PARM_2:
	.ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define _src "_strcpy_PARM_2"
	.define _dst "___SDCC_m6502_ret0"
	
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

_strcpy:
	sta	*_dst+0
	stx	*_dst+1
;//	lda	_strcpy_PARM_2+0
;//	sta	*_src+0
;//	ldx	_strcpy_PARM_2+1
;//	stx	*_src+1

	ldy	#0
cpy_loop:
	lda	[*_src],y
	sta	[*_dst],y
	beq	end
	iny
	bne	cpy_loop
	inc	*_src+1
	inc	*_dst+1
	bne	cpy_loop
;	jmp	cpy_loop
end:
	lda	*_dst+0
	rts
