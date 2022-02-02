	.include	"global.s"

	.title	"System clock"
	.module	Clock

        .globl  .sys_time

	.area	_HOME

_clock::
	LD	HL,(.sys_time)
	RET
