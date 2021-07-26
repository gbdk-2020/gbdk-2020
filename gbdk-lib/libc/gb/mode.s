	.include	"global.s"

	.title	"screen modes"
	.module	Modes

	;; BANKED:	checked
	.area	_HOME

_mode::
	LDA	HL,2(SP)	; Skip return address
	LD	L,(HL)
	LD	H,#0x00

.set_mode::
	LD	A,L
	LD	(.mode),A

	;; AND to get rid of the extra flags
	AND	#0x03
	LD	L,A
	LD	BC,#.MODE_TABLE
	SLA	L		; Multiply mode by 4
	SLA	L
	ADD	HL,BC
	JP	(HL)		; Jump to initialization routine

_get_mode::
	LD	HL,#.mode
	LD	E,(HL)
	RET
