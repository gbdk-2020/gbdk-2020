	.include	"global.s"


	.area	_HEADER_LCD (ABS)

	.org	0x48		; LCD
.int_LCD:
	JP	.int_lcd_handler

	.area	_GSINIT

	XOR	A
	LD	HL,#.int_0x48
	LD 	C,#0x08
	RST	0x28

	.area	_BASE

.int_lcd_handler:
	PUSH	AF
	PUSH	HL
	PUSH	BC
	PUSH	DE
	LD	HL, #.int_0x48 + 0
	PUSH    HL		; for stack compatibility with std handler only!
	LD	A,(HL+)
	LD      H,(HL)
	LD	L,A
	OR	H
	JR 	Z, 1$
	RST     0x20		; .call_hl
	LD	HL, #.int_0x48 + 2
	LD	A,(HL+)
	LD      H,(HL)
	LD	L,A
	OR      H
	JR      Z, 1$
	RST     0x20		; .call_hl
	LD	HL, #.int_0x48 + 4
	LD	A,(HL+)
	LD      H,(HL)
	LD	L,A
	OR      H
	CALL    NZ, .call_hl
1$:	
	POP     HL
	POP	DE
	POP	BC
	POP	HL

	;; we return at least at the beginning of mode 2
	WAIT_STAT

	POP	AF
	RETI

_add_LCD::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.add_LCD
	POP	BC
	RET

.add_LCD::
	LD	HL,#.int_0x48
	JP	.add_int

_remove_LCD::
	PUSH	BC
	LDA	HL,4(SP)	; Skip return address and registers
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	.remove_LCD
	POP	BC
	RET

.remove_LCD::
	LD	HL,#.int_0x48
	JP	.remove_int

	.area	_BSS

.int_0x48::
	.blkw	0x08
