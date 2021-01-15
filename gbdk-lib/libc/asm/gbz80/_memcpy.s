	.area	_BASE

; void *memcpy(void *dest, const void *source, int count)
___memcpy::
_memcpy::
	lda	hl,7(sp)
	ld	a,(hl-)
	ld	d, a
	ld	a,(hl-)
	ld	e,a		; de == count
	or	d
	jr	z, 6$

	push	bc		; preserve bc (smallc convention)

	ld	a,(hl-)
	ld	b,a
	ld	a,(hl-)
	ld	c,a		; bc == source
	ld	a,(hl-)
	ld	l,(hl)
	ld	h,a		; hl == dest

	srl	d
	rr	e
	jr	nc,4$
	ld	a,(bc)		; copy one byte
	ld	(hl+),a
	inc	bc
4$:
	srl	d
	rr	e
	jr	nc,5$
	ld	a,(bc)		; copy two bytes
	ld	(hl+),a
	inc	bc
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
5$:	
	inc	d
	inc	e
	jr	2$
1$:
	ld	a,(bc)		; copy four bytes
	ld	(hl+),a
	inc	bc
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
2$:
	dec	e
	jr	nz,1$
	dec	d
	jr	nz,1$
3$:
	pop	bc		; restore bc
6$:
	lda	hl,2(sp)	; return dest
	ld	a,(hl+)
	ld      e,a
	ld      d,(hl)
	ret
