	.include	"global.s"

	;; BANKED: checked
	.area	_BASE

	;; Initialize window tile table with B
.init_wtt::
	LDH	A,(.LCDC)
	BIT	6,A
	JR	Z,.is98
	JR	.is9c
	;; Initialize background tile table with B
.init_btt::
	LDH	A,(.LCDC)
	BIT	3,A
	JR	NZ,.is9c
.is98:
	LD	HL,#0x9800	; HL = origin
	JR	.init_tt
.is9c:
	LD	HL,#0x9C00	; HL = origin

.init_tt::
	LD	DE,#0x0400	; One whole GB Screen

.init_vram::
1$:
	SRL	D
	RR	E
	JR	NC, 2$
	WAIT_STAT
	LD	A, B
	LD	(HL+),A
2$:
	SRL	D
	RR	E
	JR	NC, 3$
	WAIT_STAT
	LD	A, B
	LD	(HL+),A
	LD	(HL+),A
3$:
	LD	A, D
	OR	E
	RET	Z
	
	INC	D
	INC	E
	JR	5$
4$:
	WAIT_STAT
	LD	A, B
	LD	(HL+),A
	LD	(HL+),A
	LD	(HL+),A
	LD	(HL+),A
5$:
	DEC	E
	JR	NZ, 4$
	DEC	D
	JR	NZ, 4$
	
	RET
