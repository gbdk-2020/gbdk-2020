	.include	"global.s"

	;; BANKED: checked
	.area	_BASE
	;; Copy part (size = DE) of the VRAM from (BC) to (HL)
.copy_vram::
1$:
	LDH	A,(.STAT)
	AND	#0x02
	JR	NZ,1$

	LD	A,(BC)
	LD	(HL+),A
	INC	BC
	DEC	DE
	LD	A,D
	OR	E
	JR	NZ,1$
	RET

_set_data::
_get_data::
	PUSH	BC

	LDA	HL,9(SP)	; Skip return address and registers
	LD	A,(HL-)		; DE = len
	LD	D, A
	LD	A,(HL-)
	LD	E, A
	LD	A,(HL-)		; BC = src
	LD	B, A
	LD	A,(HL-)
	LD	C, A
	LD	A,(HL-)		; HL = dst
	LD	L,(HL)
	LD	H,A

	CALL	.copy_vram

	POP	BC
	RET
