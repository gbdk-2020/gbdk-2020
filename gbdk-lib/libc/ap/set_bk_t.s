	.include	"global.s"

	.globl	.set_xy_btt
	;; BANKED:	checked
	.area	_BASE

_set_bkg_tiles::
	PUSH	BC

	LDA	HL,4(SP)	; Skip return address and registers
	LD	A,(HL+)		; D = x
	LD	D, A
	LD	E,(HL)		; E = y
	LDA	HL,9(SP)
	LD	A,(HL-)		; BC = tiles
	LD	B, A
	LD	A,(HL-)
	LD	C, A
	LD	A,(HL-)		; A = h
	LD	H,(HL)		; H = w
	LD	L,A		; L = h

	CALL	.set_xy_btt

	POP	BC
	RET
