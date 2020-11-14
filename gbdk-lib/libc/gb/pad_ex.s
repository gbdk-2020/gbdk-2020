	.include	"global.s"

	.area	_BASE

_joypad_init::	
	call	_sgb_check
	ld	a, e
	or	a
	jr	Z, 1$

	ldhl	sp, #2
	ld	a, (hl)

	cp	a, #0x02
	jr	Z, 3$
	cp	a, #0x04
	jr	Z, 4$
	jr	5$

3$:
	ld	hl, #.MLT_REQ_2
	call	.sgb_transfer
	call	.sgb_wait4
	ld	e, #0x02
	jr	2$

4$:
	ld	hl, #.MLT_REQ_4
	call	.sgb_transfer
	call	.sgb_wait4
	ld	e, #0x04
	jr	2$
5$:
	ld	hl, #.MLT_REQ_1
	call	.sgb_transfer
	call	.sgb_wait4
1$:
	ld	e, #0x01
2$:
	ldhl	sp, #3
	ld	a, (hl+)
	ld	h, (hl)
	ld	l, a
	ld	a, e
	ld	(hl+), a
	xor	a
	ld	(hl+), a
	ld	(hl+), a
	ld	(hl+), a
	ld	(hl), a	
	ret

_joypad_ex::
	lda	hl, 2(sp)
	ld	a, (hl+)
	ld	e, a
	ld	d, (hl)

.joypad_ex::
	ld	a, (de)
	inc	de
	or	a
	jr	z, 2$

	dec	a
	and	#3
	inc	a
2$:
	push	bc
	ld	b, a
1$:
	ld	a, #0x30
	ldh	(#.P1), a
	ldh	a, (#.P1)
	and	#0x0f
	sub	#0x0f
	cpl
	inc	a		; A contains joypad number

	and	#3		; buffer overrun protection

	add	e		; HL = DE + A
	ld	l, a
	adc	d
	sub	l
	ld	h, a

	ld	a, #0x20
	ldh	(#.P1), a
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	and	#0x0f
	ld	c, a
	ld	a, #0x10
	ldh	(#.P1), a
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	and	#0x0f
	swap	a
	or	c
	cpl
	ld	(hl), a

	dec	b
	jr	nz, 1$
	pop	bc

	ret 
