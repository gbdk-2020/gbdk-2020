;--------------------------------------------------------------------------
;  mul.s
;
;  Copyright (C) 2000, Michael Hope
;  Copyright (C) 2021-2022, Sebastian 'basxto' Riedel (sdcc@basxto.de)
;  Copyright (c) 2021, Philipp Klaus Krause
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

        ;; Originally from GBDK by Pascal Felber.
        ;; Updated by Phidias618.
        
        .module mul
        .area   _CODE

.globl  __mulsuchar
.globl  __muluschar
.globl  __mulschar
.globl  __muluchar
.globl  __mulint

; operands with different sign

__muluschar:
	ld b, a
	ld a, e
	ld e, b		
__mulsuchar:	
	; sign extends E while preserving A
	ld l, #0
	ld d, l
	bit 7, e
	jr z, .mul_acc_adel
	dec d
	jr .mul_acc_adel
        
__muluchar:
	ld l, #0
	ld d, l
	jr .mul_acc_adel
__mulschar:
        ; sign extends A into BC
	ld c, a
	add a
	sbc a
	ld b, a
        
	; sign extends E into DE
	ld a, e
	add a
	sbc a
	ld d, a
        ; Fall through __mulint
__mulint:
	; computes BC * DE by using the following identity :
	; BC * DE = (B * E * 256) + (C * DE)
	
	; if D = 0 computes E * BC instead
	ld a, d
	OR a
	jr z, shortcut_swap
	
	; computes B * E
	xor a
	sla b
	jr nc, 0$
	add e
0$:
	; skips the rest of the loop if either B = 0 or (B is odd and E = 0)
	jr z, .mul_acc_cdea
.irp label, 1$, 2$, 3$, 4$, 5$, 6$, 7$
	add a
	sla b
	jr nc, label
	add e
label:
.endm
	; B * E is now stored in A
	
.mul_acc_cdea:
	; computes (C * DE) + (256 * A)
	ld l, a
	ld a, c
.mul_acc_adel:
	; computes (A * DE) + (256 * L)
.irp label, 0$, 1$, 2$, 3$, 4$, 5$, 6$, 7$
	add hl, hl
	add a
	jr nc, label
	add hl, de
label:
.endm
	ld b, h
	ld c, l
	ret

shortcut_swap:
	ld l, a                ; a = 0
	ld a, e
	ld d, b
	ld e, c
	jr .mul_acc_adel

