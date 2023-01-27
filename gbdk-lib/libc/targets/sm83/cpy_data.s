	.include	"global.s"

	.area   _CODE

	.globl	_vmemcpy2, _set_data2, _get_data2

_vmemcpy2::
_set_data2::
_get_data2::
	;dest in de
	;src in bc
	;n in sp+2,sp+3
	ldhl	sp, #3
	ld	a, (hl-)
	ld	l, (hl)
	ld	h, b
	ld	b, a
	ld	a, l
	ld	l, c
	srl	b
	rra
	ld	c, a

	;dest in de (backup in sp+0,sp+1)
	;src in hl
	;n/2 in bc
	;LSB of bc in carry
	jr nc, skip_one

	WAIT_STAT
	ld	a, (hl+)
	ld	(de), a
	inc	de
skip_one:
	;n/2 in bc
	;shift second LSB to carry
	srl	b
	rr	c
	;n/4 in bc
	inc	b
	inc	c
	jr nc, test
	jr	copy_two
.irp	idx,copy_four,copy_two
	idx:
	.rept	2
		WAIT_STAT
		ld	a, (hl+)
		ld	(de), a
		inc	de
	.endm
.endm
test:
.irp	idx,c,b
	dec	idx
	jr	nz, copy_four
.endm

	;get return address
	pop	hl
	;throw away n
	pop	af
	jp	(hl)
