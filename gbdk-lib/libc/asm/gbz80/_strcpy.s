	.area	_BASE

; char *strcpy(char *dest, const char *source)
_strcpy::
	lda	hl,2(sp)
	ld	a,(hl+)
	ld	e, a
	ld	a,(hl+)
	ld	d, a
	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	push	de
1$:	
	ld	a,(hl+)
	ld	(de),a
	inc	de
	or	a,a
	jr	nz,1$

	pop	de
	ret
	