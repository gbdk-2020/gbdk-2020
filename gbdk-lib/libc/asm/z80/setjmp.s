;--------------------------------------------------------------------------
;  setjmp.s
;
;  Copyright (C) 2026, Phidias618
;
;--------------------------------------------------------------------------

	.area   _CODE

	.globl ___setjmp

___setjmp:
	; Store frame pointer.
	push ix
	pop de
	ld (hl), e
	inc hl
	ld (hl), d
	inc hl

	; Store stack pointer.
	ex hl, de
	ld hl, #2
	add hl, sp
	ex hl, de

	ld (hl), e
	inc hl
	ld (hl), d
	inc hl

	pop de		; pop return address

	; Store return address.
	ld (hl), e
	inc hl
	ld (hl), d

	; return 0
	ex hl, de
	ld de, #0
	jp (hl)
	
.globl _longjmp
_longjmp:
	pop af				; Discard return address.

	; Ensure that return value is non-zero.
	ld a, d
	or e
	jr nz, 0$
	inc d
0$:
	ld b, d
	ld c, e

	; Restore frame pointer.
	ld e, (hl)
	inc hl
	ld d, (hl)
	inc hl
	push de
	pop ix

	; Restore stack pointer.
	ld e, (hl)
	inc hl
	ld d, (hl)
	inc hl
	ex hl, de
	ld sp, hl
	ex hl, de

	ld e, (hl)
	inc hl
	ld d, (hl)
	
	ex hl, de
	ld d, b
	ld e, c
	jp (hl)
	
