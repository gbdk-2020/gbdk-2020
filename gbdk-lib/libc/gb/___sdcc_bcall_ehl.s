	.include	"global.s"

	.area _BASE

___sdcc_bcall_ehl::			; Performs a long call.
ldh a,(__current_bank)
	push	af			; Push the current bank onto the stack
	ld	a, e
	ldh	(__current_bank),a
	ld	(.MBC1_ROM_PAGE),a	; Perform the switch
	rst	0x20
	pop	af			; Pop the old bank
	ld	(.MBC1_ROM_PAGE),a
	ldh	(__current_bank),a
	ret