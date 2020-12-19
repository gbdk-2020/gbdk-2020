	.area	_BASE

; void *memset (void *s, int c, size_t n)
_memset::
	lda	hl,7(sp)
	ld	a,(hl-)
	ld 	d, a
	ld	a,(hl-)
	ld	e, a
	or	d
	jr	z,6$
	
	dec	hl
	ld	a,(hl-)
	push	af
	ld	a,(hl-)
	ld	l,(hl)
	ld	h,a
	pop	af
	
	srl	d
	rr	e
	jr	nc,4$
	ld	(hl+),a
4$:	
	srl	d
	rr	e
	jr	nc,5$
	ld	(hl+),a
	ld	(hl+),a
5$:		
	inc	d
	inc	e
	jr	2$
1$:	
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
2$:
	dec	e
	jr	nz,1$
	dec	d
	jr	nz,1$
6$:
	lda	hl,2(sp)
	ld	a,(hl+)
	ld	e,a
	ld	d,(hl)
	ret
