	.area	_BASE

; int strcmp(const char *s1, const char *s2) 
_strcmp::
	lda	hl,2(sp)
	ld	a,(hl+)
	ld	e, a
	ld	a,(hl+)
	ld	d, a
	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	jr	1$
2$:	
	ld	a,(de)
	sub	(hl)
	jr	nz,4$
	;; A == 0
	cp	(hl)
	jr	z,3$
1$:	
	inc	de
	inc	hl
	jr	2$

3$:
	ld	de,#0
	jr	5$
4$:
	ld	de,#1
	jr	nc,5$
	ld	de,#-1
5$:
	ret
	