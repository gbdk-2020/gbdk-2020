	;; Implementation of some string functions in
	;; assembler.

	;; Why - because I want a better dhrystone score :)
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

; void *memcpy(void *dest, const void *source, int count)
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

	inc	b
	inc	c
	push	hl

	jr	2$
1$:
	ld	a,(de)
	ld	(hl+),a
	inc	de
2$:
	dec	c
	jr	nz,1$
	dec	b
	jr	nz,1$	

	pop	de
	pop	bc
	ret

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

1$:	
	ld	a,(de)
	sub	(hl)		; s1[i]==s2[i]?
	jr	nz,2$		; -> Different
	;; A == 0
	cp	(hl)		; s1[i]==s2[i]==0?
	inc	de
	inc	hl
	jr	nz,1$		; ^ Didn't reach a null character. Loop.
	; Yes. return 0;
	ld	d,a		; Since a == 0 this is faster than a 16 bit immediate load.
	ld	e,a
	ret

2$:
	ld	de,#-1
	ret	c	
	ld	de,#1
	ret
	