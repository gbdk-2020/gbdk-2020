	.include	"global.s"

	.area	_HOME

_fill_win_rect::
	PUSH	BC

	LDA	HL,4(SP)	; Skip return address and registers
	LD	A,(HL+)		; D = x
	LD	D, A
	LD	E,(HL)		; E = y
	LDA	HL,8(SP)
	LD	A,(HL-)		; B = tile
	LD	B, A
	LD	A,(HL-)		; A = h
	LD	H,(HL)		; H = w
	LD	L,A		; L = h

	CALL	.fill_rect_wtt

	POP	BC
	RET
