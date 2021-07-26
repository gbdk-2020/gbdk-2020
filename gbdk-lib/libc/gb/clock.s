	.include	"global.s"

	.title	"System clock"
	.module	Clock

	;; BANKED:	checked
	.area	_HOME

_clock::
	LD	HL,#.sys_time
	DI
	LD	A,(HL+)
	EI			;; Interrupts are disabled for the next instruction...
	LD	D,(HL)
	LD	E,A
	RET
