	.area	_BASE

; void *memcpy(void *dest, const void *source, int count)
.memcpy::
	push	bc
	jr	.memcpy_cont
.memcpy_exit:
	pop	hl
	pop	de
	push 	de
	jp	(hl)
___memcpy::
_memcpy::
	lda	hl,7(sp)
	ld	a,(hl-)
	ld	d, a
	ld	a,(hl-)
	or	d
	jr	z,.memcpy_exit
	ld	e,a
	push	bc
	ld	a,(hl-)
	ld	b,a
	ld	a,(hl-)
	ld	c,a
	ld	a,(hl-)
	ld	l,(hl)
	ld	h,a
.memcpy_cont:	
	push	hl

	ld	a, e
	and	#0xfc
	or	d
	jr	z,3$

	push	de

	srl	d
	rr	e
	srl	d
	rr	e

	inc	d
	inc	e
	jr	2$
1$:
	ld	a,(bc)
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

	pop	de
3$:
	srl	e
	jr	nc,4$
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
4$:
	srl	e
	jr	nc,5$
	ld	a,(bc)
	ld	(hl+),a
	inc	bc
	ld	a,(bc)
	ld	(hl+),a
5$:	
	pop	de
	pop	bc
	ret
