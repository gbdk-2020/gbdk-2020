	.include	"global.s"

	.module sio

	.globl	.int

	.area	_HEADER_SIO (ABS)

	.org	0x58		; SIO
.int_SIO:
	PUSH	AF
	PUSH	HL
	LD	HL,#.int_0x58
	JP	.int

	.area	_HOME

_add_SIO::
.add_SIO::
	LD	HL,#.int_0x58
	JP	.add_int

_remove_SIO::
.remove_SIO::
	LD	HL,#.int_0x58
	JP	.remove_int

	.area	_DATA

.int_0x58::
	.blkw	0x05
