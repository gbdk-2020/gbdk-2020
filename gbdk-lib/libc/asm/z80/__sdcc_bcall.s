;--------------------------------------------------------------------------
;  __sdcc_bcall.s
;
;  Copyright (C) 2011, Maarten Brock, 2015 Philipp Klaus Krause
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

; This file contains generic trampolines for banked function calls.
; They are not complete. Programmer must provide set_bank and get_bank
; routines. Or rewrite whole code completely.
;
	.area   _CODE

	.globl ___sdcc_bcall
	.globl ___sdcc_bcall_abc
	.globl ___sdcc_bcall_ehl
;
; trampoline to call banked functions
; used when legacy banking is enabled only
; Usage:
;   call ___sdcc_bcall
;   .dw  <function>
;   .dw  <function_bank>
;
___sdcc_bcall::
	ex	(sp), hl
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	inc	hl
	ld	a, (hl)
	inc	hl
	inc	hl
	ex	(sp), hl
;
; trampoline to call banked functions with __z88dk_fastcall calling convention
; Usage:
;  ld   a, #<function_bank>
;  ld   bc, #<function>
;  call ___sdcc_bcall_abc
;
___sdcc_bcall_abc::
	push	hl
	ld	l, a
	call	get_bank	;must return A as current bank number, other registers expected to be unchanged
	ld	h, a
	ld	a, l
	ex	(sp), hl
	inc	sp
	call	___sdcc_bjump_abc
	dec	sp
	pop	af
	jp	set_bank
;
___sdcc_bjump_abc:
	call	set_bank	;set current bank to A, other registers expected to be unchanged
	push	bc
	ret
;
; default trampoline to call banked functions
; Usage:
;  ld   e, #<function_bank>
;  ld   hl, #<function>
;  call ___sdcc_bcall_ehl
;
___sdcc_bcall_ehl::
	call	get_bank
	push	af
	inc	sp
	call	___sdcc_bjump_ehl
	dec	sp
	pop	af
	jp	set_bank
;
___sdcc_bjump_ehl:
	ld	a, e
	call	set_bank
	jp	(hl)
