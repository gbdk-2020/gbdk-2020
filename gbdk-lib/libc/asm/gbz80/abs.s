; int abs(int)
_abs::
	; first word is return address
	; second is argument
	pop hl
	pop de ; make copy of de
	push de ; but leave it on the stack for the caller to clean up
	ld a, d
	add a, a
	jr nc, .done
	rra
	cpl
	ld d, a
	ld a, e
	cpl
	ld e, a
	inc de
.done:
	jp (hl)
