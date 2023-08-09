	.include	"global.s"

	.area   _CODE

	.globl	_vmemcpy, _set_data, _get_data, .copy_vram

;DE: dest
;HL: src
;BC: len
.copy_vram::
	push	bc
	ld      a, c
	call	cpy_vram
	ret

;DE: dest
;BC: src
;sp+2: len
_vmemcpy::
_set_data::
_get_data::
	ldhl	sp, #3
	ld	a, (hl-)
	ld	l, (hl)
	ld	h, b
	ld	b, a
	ld	a, l
	ld	l, c

;DE: dest
;HL: src
;B,A: len
cpy_vram:
	srl	b
	rra
	ld	c, a
	jr	nc, skip_one

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
	jr	nc, test
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
