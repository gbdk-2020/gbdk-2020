	.include	"global.s"

	.title	"screen modes"
	.module	Modes

	.area	_HOME

_mode::
	LD	(.mode), A
	AND	#0x03
	ADD     A
	ADD     A
	LD	L,A
	LD      H, #0
	LD	BC, #.MODE_TABLE
	ADD	HL, BC
	JP	(HL)		; Jump to initialization routine

_get_mode::
	LD	A, (.mode)
	RET
