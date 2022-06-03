	.include	"global.s"

	.MLT_REQ	= 0x11

	.area	_HOME

_joypad_init::	
	call	.sgb_check
	ld	a, e
	inc     e
	or	a

	jr	Z, 1$

	ldhl	sp, #0x02
	ld	e, (hl)

	add	sp, #-0x10
	ldhl	sp, #0
	ld	c, #0x10
	xor 	a
	rst	0x28

	ldhl	sp, #0
	ld	a, #((.MLT_REQ << 3) | 1)
	ld	(hl+), a

	ld	a, e

	cp	a, #0x02
	jr	Z, 3$
	cp	a, #0x04
	jr	Z, 4$
	jr	5$

3$:
	ld	a, #0x01
	ld	(hl-), a
	call	.sgb_transfer
	ld	e, #0x02
	jr	2$

4$:
	ld	a, #0x03
	ld	(hl-), a
	call	.sgb_transfer
	ld	e, #0x04
	jr	2$
5$:
	ld	a, #0x00
	ld	(hl-), a
	call	.sgb_transfer
	ld	e, #0x01
2$:
	add	sp, #0x10
1$:
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
	ld	a, #(.P14 | .P15)
	ldh	(#.P1), a
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	and	#0x0f
	sub	#0x0f
	cpl
	inc	a		; A contains joypad number

	and	#0x03		; buffer overrun protection

	add	e		; HL = DE + A
	ld	l, a
	adc	d
	sub	l
	ld	h, a

	ld	a, #.P15
	ldh	(#.P1), a
	ldh	a, (#.P1)
	ldh	a, (#.P1)
	and	#0x0f
	ld	c, a
	ld	a, #.P14
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
