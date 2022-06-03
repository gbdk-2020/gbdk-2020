	.include	"global.s"

	.area	_HOME

	;; Wait until all buttons have been released
.padup::
_waitpadup::
	PUSH	AF		; Save modified registers
	PUSH	DE
1$:
	LD	D,#0x7F		; wait for .jpad return zero 127 times in a row
2$:
	CALL	.jpad
	OR	A		; Have all buttons been released?
	JR	NZ,1$		; Not yet

	DEC	D
	JR	NZ,2$
	
	POP	DE
	POP	AF
	RET

	;; Get Keypad Button Status
	;; The following bits are set if pressed:
	;;   0x80 - Start   0x08 - Down
	;;   0x40 - Select  0x04 - Up
	;;   0x20 - B	    0x02 - Left
	;;   0x10 - A	    0x01 - Right
_joypad::
.jpad::
	LD	A,#.P15
	LDH	(.P1),A		; Turn on P15

	LDH	A,(.P1)		; Delay
	LDH	A,(.P1)
	AND	#0x0F
	LD	E,A
	LD	A,#.P14
	LDH	(.P1),A		; Turn on P14
	LDH	A,(.P1)		; Delay
	LDH	A,(.P1)
	LDH	A,(.P1)
	LDH	A,(.P1)
	LDH	A,(.P1)
	LDH	A,(.P1)
	AND	#0x0F
	SWAP	A
	OR	E
	CPL
	LD	E,A
	LD	A,#(.P14 | .P15)
	LDH	(.P1),A		; Turn off P14 and P15 (reset joypad)
	LD	A,E
	RET

	;; Wait for the key to be pressed
_waitpad::
	LD	D,A
	;; Wait for the key in D to be pressed
.wait_pad::
1$:
	CALL	.jpad		; Read pad
	AND	D		; Compare with mask?
	JR	Z,1$		; Loop if no intersection
	RET

