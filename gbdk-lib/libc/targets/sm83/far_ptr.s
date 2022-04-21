;--------------------------------------------------------------------------
;  far_ptr.s
;
;  Copyright (C) 2020, Tony Pavlov
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

	.module	far_ptr

	.include	"global.s"
	
	.area	_HOME
		
___call__banked::
	ldh	A, (#__current_bank)
	push	AF
	ld	HL, #1$
	push	HL
	ld	HL, #___call_banked_addr
	ld	A, (HL+)
	ld	H, (HL)
	ld	L, A
	ld	A, (#___call_banked_bank)
	ldh	(#__current_bank), A
	ld	(.MBC_ROM_PAGE),A
	jp	(HL)
1$:
	pop	AF
	ldh	(#__current_bank), A
	ld	(.MBC1_ROM_PAGE), A
	ret

_to_far_ptr::
	lda	HL, 2(SP)
	ld	A, (HL+)
	ld	E, A
	ld	A, (HL+)
	ld	D, A
	ld	A, (HL+)
	ld	H, (HL)
	ld	L, A
	ret

	.area	_DATA
	
___call_banked_ptr::
___call_banked_addr::
	.ds	0x02		; far pointer offset
___call_banked_bank::
	.ds	0x02		; far pointer segment
