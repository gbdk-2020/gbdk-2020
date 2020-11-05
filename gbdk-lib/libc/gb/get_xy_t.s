	.include        "global.s"

	;; BANKED:	checked
	.area	_BASE

	;; Store window tile table into (BC) at xy = DE of size WH = HL
.get_xy_wtt::
	PUSH	HL		; Store WH
	LDH	A,(.LCDC)
	BIT	6,A
	JR	Z,.is98
	JR	.is9c
	;; Store background tile table into (BC) at XY = DE of size WH = HL
.get_xy_btt::
	PUSH	HL		; Store WH
	LDH	A,(.LCDC)
	BIT	3,A
	JR	NZ,.is9c
.is98:
	LD	HL,#0x9800
	JR	.get_xy_tt
.is9c:
	LD	HL,#0x9C00
	
	;; Store background tile table into (BC) at XY = DE, size WH on stack, from vram from address (HL)
.get_xy_tt::
	PUSH	BC		; Store source

	SWAP	E
	RLC	E
	LD	A,E
	AND	#0x03
	ADD	H
	LD	B,A
	LD	A,#0xE0
	AND	E
	ADD	D
	LD	C,A		; dest BC = HL + 0x20 * Y + X

	POP	HL		; HL = source
	POP	DE		; DE = WH
	PUSH	DE		; store WH
	PUSH	BC		; store dest

3$:				; Copy W tiles

	WAIT_STAT
	LD	A, (BC)
	LD	(HL+), A
	
	LD	A, C		; inc dest and wrap around
	AND	#0xE0
	LD	E, A
	LD	A, C
	INC	A
	AND	#0x1F
	OR	E
	LD	C, A

	DEC	D
	JR	NZ, 3$

	POP	BC
	POP	DE

	DEC	E
	RET	Z

	PUSH	DE

	LD	A, B		; next row and wrap around
	AND	#0xFC
	LD 	E, A		; save high bits

	LD	A,#0x20

	ADD	C
	LD	C, A
	ADC	B
	SUB	C
	AND	#0x03
	OR	E		; restore high bits
	LD	B, A

	PUSH	BC
	
	JR	3$
