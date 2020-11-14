	.include        "global.s"

	;; BANKED:	checked
	.area	_BASE

_get_tiles::
	PUSH	BC

	LDA	HL,11(SP)	; Skip return address and registers
	LD	A,(HL-)		; DE = src
	LD	D, A
	LD	A,(HL-)
	LD	E, A
	LD	A,(HL-)		; BC = dst
	LD	B, A
	LD	C,(HL)
	LDA	HL,4(SP)	; Skip return address and registers
	PUSH	DE		; Store address on stack for set_xy_tt
	LD	A,(HL+)		; D = x
	LD	D, A
	LD	A,(HL+)		; E = y
	LD	E, A
	LD	A,(HL+)		; A = w
	LD	L,(HL)		; L = h
	LD	H,A		; H = w

	CALL	.get_xy_tt

	POP	BC
	RET
