	.include	"global.s"

	.globl	.int

	.area	_HEADER_JOY (ABS)

	.org	0x60		; JOY
.int_JOY:
	PUSH	AF
	PUSH	HL
	LD	HL,#.int_0x60
	JP	.int

	.area	_BASE

_add_JOY::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.add_JOY
	POP	BC
	RET

.add_JOY::
	LD	HL,#.int_0x60
	JP	.add_int

_remove_JOY::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.remove_JOY
	POP	BC
	RET

.remove_JOY::
	LD	HL,#.int_0x60
	JP	.remove_int

	.area	_BSS

.int_0x60::
	.blkw	0x08
