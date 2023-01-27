	.include	"global.s"

	.globl	.int

	.area	_HEADER_JOY (ABS)

	.org	0x60		; JOY
.int_JOY:
	PUSH	AF
	PUSH	HL
	LD	HL,#.int_0x60
	JP	.int

	.area	_HOME

_add_JOY::
.add_JOY::
	LD	HL,#.int_0x60
	JP	.add_int

_remove_JOY::
.remove_JOY::
	LD	HL,#.int_0x60
	JP	.remove_int

	.area	_DATA

.int_0x60::
	.blkw	0x05
