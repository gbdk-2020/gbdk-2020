	.include "global.s"

	.title	"CGB support"
	.module	CGB

	;; BANKED: checked, imperfect
	.area	_BASE

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

	.area	_CODE
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

	.area	_CODE
_cpu_slow::			; Banked
	LDH	A,(.KEY1)
	AND	#0x80		; Is GBC in double speed mode?
	RET	Z		; No, already in single speed

shift_speed:
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
        JR	shift_speed


_cgb_compatibility::		; Banked
	LD	HL, #1$
	PUSH	HL
	LD	HL, #0x0100
	PUSH 	HL
	CALL	_set_sprite_palette
	CALL	_set_bkg_palette
	ADD	SP, #4
	RET
1$:
	.DW	0xFF7F, 0x56B5, 0x294A, 0x0000
