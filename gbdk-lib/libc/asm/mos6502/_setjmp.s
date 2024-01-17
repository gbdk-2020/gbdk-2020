;-------------------------------------------------------------------------
;   setjmp.s - source file for ANSI routines setjmp & longjmp
;
;   Copyright (C) 2020, Steven Hugg. hugg@efasterlight.com
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

	.module _setjmp

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___setjmp    ; 
        .globl _longjmp
        .globl _longjmp_PARM_2
	
;--------------------------------------------------------
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
_longjmp_PARM_2:
        .ds 2

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define ptr "DPTR"
	.define rv "_longjmp_PARM_2"

;--------------------------------------------------------
; code
;--------------------------------------------------------
        .area _CODE

;------------------------------------------------------------
; int __setjmp (jmp_buf buf)
;------------------------------------------------------------

___setjmp:
        stx	*(ptr + 1)		; msb(buf)
        sta	*(ptr + 0)		; lsb(buf)

        ; save stack pointer
        tsx
        ldy	#0
        txa
        sta	[ptr],y

        ; save return address
        lda	0x101,x
        iny
        sta	[ptr],y
        lda	0x102,x
        iny
        sta	[ptr],y

        ; return 0
        lda	#0
        tax
        rts

;------------------------------------------------------------
; int longjmp (jmp_buf buf, int rv)
;------------------------------------------------------------

_longjmp:
        stx	*(ptr + 1)		; msb(buf)
        sta	*(ptr + 0)		; lsb(buf)

        ; restore stack pointer
        ldy	#0
        lda	[ptr],y
        tax
        txs

        ; set return address
        iny
        lda	[ptr],y
        sta	0x101,x
        iny
        lda	[ptr],y
        sta	0x102,x

;_setjmp.c:224: return rv ? rv : 1;
        ldx    *(rv + 1)
        txa
        ora    *(rv + 0)
        beq     0001$
        lda    *(rv + 0)
        rts
0001$:
        lda     #0x01
        rts
