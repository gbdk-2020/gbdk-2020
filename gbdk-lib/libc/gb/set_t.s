	.include	"global.s"

	.globl .set_xy_tt

	;; BANKED:	checked, imperfect
	.area	_BASE

_set_tiles::
	PUSH	BC

	LDA	HL,11(SP)	; Skip return address and registers
	LD	A,(HL-)		; BC = src
	LD	B, A
	LD	A,(HL-)
	LD	C, A
	LD	A,(HL-)		; DE = dst
	LD	D, A
	LD	E,(HL)
	LDA	HL,4(SP)	; Skip return address and registers
	PUSH	DE		; Store address on stack for set_xy_tt
	LD	A,(HL+)		; D = x
	LD	D, A
	LD	A,(HL+)		; E = y
	LD	E, A
	LD	A,(HL+)		; A = w
	LD	L,(HL)		; L = h
	LD	H,A		; H = w

	CALL	.set_xy_tt

	POP	BC
	RET
