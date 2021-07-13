	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.area	_CODE
_cpu_slow::			; Banked
	LDH	A,(.KEY1)
	AND	#0x80		; Is GBC in double speed mode?
	RET	Z		; No, already in single speed

.shift_speed:
	LDH	A,(.IE)
	PUSH	AF

	XOR	A		; A = 0
	LDH	(.IE),A		; Disable interrupts
	LDH	(.IF),A

	LD	A,#0x30
	LDH	(.P1),A

	LD	A,#0x01
	LDH	(.KEY1),A

	STOP

	POP	AF
	LDH	(.IE),A

	RET

_cpu_fast::			; Banked
	LDH	A,(.KEY1)
	AND	#0x80		; Is GBC in double speed mode?
	RET	NZ		; Yes, exit
        JR	.shift_speed
