	.area	_BASE

; void *memcpy(void *dest, const void *source, int count)
___memcpy::
_memcpy::
	push	bc
	lda	hl,6(sp)
	ld	a,(hl+)
	ld	e, a
	ld	a,(hl+)
	ld	d, a
	ld	a,(hl+)
	ld	c, a
	ld	b,(hl)
	lda	hl,4(sp)
	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a
	jr	.memcpy_cont
.memcpy::
	push    bc
.memcpy_cont:	
	push	hl
	push    bc

	srl     b
	rr      c
	srl     b
	rr      c
	
	ld      a,b
	or      c
	jr      z, 3$

	inc	b
	inc	c

	jr	2$
1$:
	ld	a,(de)
	ld	(hl+),a
	inc	de
	ld	a,(de)
	ld	(hl+),a
	inc	de
	ld	a,(de)
	ld	(hl+),a
	inc	de
	ld	a,(de)
	ld	(hl+),a
	inc	de
2$:
	dec	c
	jr	nz,1$
	dec	b
	jr	nz,1$	

3$:
	pop     bc
	srl     c
	jr      nc, 4$
	ld	a,(de)
	ld	(hl+),a
	inc	de	
4$:
	srl     c
	jr      nc, 5$
	ld	a,(de)
	ld	(hl+),a
	inc	de	
	ld	a,(de)
	ld	(hl+),a
5$:	
	pop	de
	pop     bc
	ret
	