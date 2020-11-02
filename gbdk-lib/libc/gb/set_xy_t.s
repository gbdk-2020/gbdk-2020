	.include	"global.s"

	.area	_BASE

	;; Set window tile table from BC at XY = DE of size WH = HL
.set_xy_wtt::
	PUSH	HL		; Store WH
	LDH	A,(.LCDC)
	BIT	6,A
	JR	Z,.is98
	JR	.is9c
	;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_xy_btt::
	PUSH	HL		; Store WH
	LDH	A,(.LCDC)
	BIT	3,A
	JR	NZ,.is9c
.is98:
	LD	HL,#0x9800
	JR	.set_xy_tt
.is9c:
	LD	HL,#0x9C00
	;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (HL)
.set_xy_tt::
	PUSH	BC		; Store source
	XOR	A
	LD	B,A
	OR	E
	JR	Z,2$

	LD	C,#0x20		; One line is 20 tiles
	SRL	E
	JR	NC,0$
	ADD	HL,BC
0$:
	JR	Z,2$
	SLA	C
1$:
	ADD	HL,BC
	DEC	E
	JR	NZ,1$
2$:
	LD	C,D
	ADD	HL,BC		; HL = HL + 0x20 * Y + X

	POP	BC		; BC = source
	POP	DE		; DE = WH
	PUSH	DE		; Store WH

3$:				; Copy W tiles
	SRL	D
	JR	NC,4$

	WAIT_STAT
	LD	A,(BC)		; transfer one byte
	LD	(HL+),A
	INC	BC
4$:
	XOR	A
	OR	D
	JR 	Z, 6$
5$:
	WAIT_STAT
	.REPT 2			; transfer two bytes
		LD	A,(BC)
		LD	(HL+),A
		INC	BC
	.ENDM
	DEC	D
	JR	NZ,5$
6$:
	POP	AF
	LD	D,A		; Restore D = W

	DEC	E

	RET	Z

	LD	A,#0x20
	SUB	D
	ADD_A_REG16 H,L

	PUSH	DE		; Store WH
	JR	3$
