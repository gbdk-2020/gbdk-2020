;--------------------------------------------------------------------------
;  labs.s
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

	.module	labs

	.area	_CODE

;long labs(long num)
_labs::
	lda	HL, 2(SP)
	ld	A, (HL+)
	ld	E, A
	ld	A, (HL+)
	ld	D, A
	ld	A, (HL+)
	ld	H, (HL)
	ld	L, A		; DEHL = num

.labs::
	ld	A, H
	add	A, A
	ret	NC

1$:
	ld	A, E
	cpl
	add	#1
	ld	E, A

	ld	A, D
	cpl
	adc	#0
	ld	D, A

	ld	A, L
	cpl
	adc	#0
	ld	L, A

	ld	A, H
	cpl
	adc	#0
	ld	H, A

	ret 
