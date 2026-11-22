	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.area	_CODE

_cpu_slow::
	LDH	A, (rKEY1)
	AND	#0x80		; Is GBC in double speed mode?
	RET	Z		; No, already in single speed

.shift_speed:
	LDH	A, (rIE)
	PUSH	AF

	XOR	A		; A = 0
	LDH	(rIE), A	; Disable interrupts
	LDH	(rIF), A

	LD	A, #0x30
	LDH	(rP1), A

	LD	A, #0x01
	LDH	(rKEY1), A

	STOP

	POP	AF
	LDH	(rIE), A

	RET

_cpu_fast::
	LDH	A, (rKEY1)
	AND	#0x80		; Is GBC in double speed mode?
	RET	NZ		; Yes, exit
        JR	.shift_speed
