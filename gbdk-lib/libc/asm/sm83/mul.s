;--------------------------------------------------------------------------
;  mul.s
;
;  Copyright (c) 2026, Phidias618
;
;--------------------------------------------------------------------------
        
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
	; skips the rest of the loop if either B = 0 or (B >= 128 and E = 0)
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

