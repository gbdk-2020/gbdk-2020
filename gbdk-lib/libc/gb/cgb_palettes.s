	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.area	_CODE

_set_sprite_palette::		; Non-banked
	PUSH	BC
	LD	C,#.OCPS
	JR	.set_palette

_set_bkg_palette::		; Non-banked
	PUSH	BC
	LD	C,#.BCPS

.set_palette::
	LDA	HL,4(SP)	; Skip return address and registers
	LD	A,(HL+)		; first_palette
	ADD	A		; A *= 8
	ADD	A
	ADD	A
	OR	#0x80		; Set auto-increment
	LDH	(C),A
	INC	C
	LD	A,(HL+)		; D = nb_palettes
	ADD	A		; A *= 8
	ADD	A
	ADD	A
	LD	B,A		; Number of bytes
	LD	A,(HL+)		; rgb_data
	LD	H,(HL)
	LD	L,A
1$:	
	WAIT_STAT

	LD	A,(HL+)
	LDH	(C),A
	DEC	B
	JR	NZ,1$

	POP	BC
	RET

_set_sprite_palette_entry::	; Banked
	PUSH	BC
	LD	C,#.OCPS
	JR	.set_palette_entry

_set_bkg_palette_entry::	; Banked
	PUSH	BC
	LD	C,#.BCPS

.set_palette_entry::
	LDA	HL,.BANKOV+2(SP); Skip return address and registers
	LD	A,(HL+)		; first_palette
	ADD	A		; A *= 4
	ADD	A
	LD	B,A
	LD	A,(HL+)		; pal_entry
	ADD	B		; A += first_palette * 4
	ADD	A		; A *= 2
	OR	#0x80		; Set auto-increment
	LDH	(C),A
	INC	C
	LD	A,(HL+)		; rgb_data
	LD	H,(HL)
	LD	L,A

	WAIT_STAT

	LD	A,L
	LDH	(C),A
	LD	A,H
	LDH	(C),A

	POP	BC
	RET
