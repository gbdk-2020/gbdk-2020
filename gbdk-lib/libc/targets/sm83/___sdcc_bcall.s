	.include	"global.s"

	.area _HOME

___sdcc_bcall::
banked_call::			; Performs a long call.
	pop	hl		; Get the return address
	ldh	a,(__current_bank)
	push	af		; Push the current bank onto the stack
	ld	a,(hl+)		; Fetch the call address
	ld	e, a
	ld	a,(hl+)
	ld	d, a
	ld	a,(hl+)		; ...and page
	inc	hl		; Yes this should be here
	push	hl		; Push the real return address
	ldh	(__current_bank),a
	ld	(.MBC_ROM_PAGE),a	; Perform the switch
	ld	l,e
	ld	h,d
	rst	0x20
banked_ret::
	pop	hl		; Get the return address
	pop	bc		; Pop the old bank
	ld c, a
	ld a, b
	ldh	(__current_bank),a
	ld	(.MBC_ROM_PAGE),a
	ld a, c		; we must preserve A for the new calling convention
	jp	(hl)
