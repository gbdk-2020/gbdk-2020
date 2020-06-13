	.area	_BASE

; void *memset (void *s, int c, size_t n)
_memset::
	lda	hl,7(sp)
	ld	a,(hl-)
	ld 	d, a
	or      (hl)
	ret     z
	ld	a,(hl-)
	ld	e, a
	dec	hl
	ld	a,(hl-)
	push    af
	ld	a,(hl-)
	ld	l,(hl)
	ld	h,a
	pop	af
.memset::
	push    bc

	ld      c,e
	srl     d
	rr      e
	srl     d
	rr      e
	
	ld      b,a
	ld      a,d
	or      e
	ld      a,b
	jr      z, 3$
	
	inc     d
	inc     e
	jr      2$
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

3$:
	srl     c
	jr      nc, 4$
	ld	(hl+),a
4$:	
	srl     c
	jr      nc, 5$
	ld	(hl+),a
	ld	(hl+),a
5$:
	pop     bc
	ret
