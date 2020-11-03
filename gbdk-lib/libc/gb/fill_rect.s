	.include	"global.s"

	.area	_BASE

.fill_rect_wtt::
	PUSH	HL
	LDH	A,(.LCDC)
	BIT	6,A
	JR	Z,.is98
	JR	.is9c
	;; Initialize background tile table with B
.fill_rect_btt::
	PUSH	HL
	LDH	A,(.LCDC)
	BIT	3,A
	JR	NZ,.is9c
.is98:
	LD	HL,#0x9800	; HL = origin
	JR	.fill_rect
.is9c:
	LD	HL,#0x9C00	; HL = origin

	;; fills rect from HL; HW: stack; Fill: B
.fill_rect:
	XOR	A
	OR	E
	LD	A, B		; save tile in A
	LD	B, #0
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

	POP	BC		; load height and width
	LD	D, #0
	LD	E, B
	LD	B, A		; restore tile to B

	INC	C
	JR	4$
3$:
	PUSH	DE
	PUSH	HL
	CALL 	.init_vram
	POP	HL
	POP	DE

	LD	A, #0x20	; next row
	ADD_A_REG16 H, L
4$:
	DEC	C
	JR	NZ, 3$

	RET