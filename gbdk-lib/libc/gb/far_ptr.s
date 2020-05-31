	.include	"global.s"

	.area	_BSS
	
___call_banked_addr::
	.ds	0x02		; far pointer offset
___call_banked_bank::
	.ds	0x02		; far pointer segment
	
	.area	_HOME
		
___call__banked::
	ldh	A, (#__current_bank)
	push	AF
	ld	HL, #caba01$
	push	HL
	ld	HL, #___call_banked_addr
	ld	A, (HL+)
	ld	H, (HL)
	ld	L, A
	ld	A, (#___call_banked_bank)
	ld	(.MBC1_ROM_PAGE),A
	ldh	(#__current_bank), A
	jp	(HL)
caba01$:
	pop	AF
	ld	(.MBC1_ROM_PAGE), A
	ldh	(#__current_bank), A
	ret

_to_far_ptr::
	lda	HL, 2(SP)
	ld	A, (HL+)
	ld	E, A
	ld	A, (HL+)
	ld	D, A
	ld	A, (HL+)
	ld	H, (HL)
	ld	L, A
	ret
