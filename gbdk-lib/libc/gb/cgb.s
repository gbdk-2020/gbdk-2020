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

	WAIT_STAT
	LD	A,#0x80
	LDH	(.BCPS),A	; Set default bkg palette
	OR	#0x7f		; 0xff: White
	LDH	(.BCPD),A
	RRA			; 0x7f
	LDH	(.BCPD),A

	WAIT_STAT
	LD	A,#0xb5		; Light gray
	LDH	(.BCPD),A
	LD	A,#0x56
	LDH	(.BCPD),A
	LD	A,#0x4a		; Dark gray
	LDH	(.BCPD),A

	WAIT_STAT
	LD	A,#0x29
	LDH	(.BCPD),A
	XOR	A		; Black
	LDH	(.BCPD),A
	LDH	(.BCPD),A

	WAIT_STAT
	LD	A,#0x80
	LDH	(.OCPS),A	; Set default sprite palette
	OR	#0x7f		; 0xff: White
	LDH	(.OCPD),A
	RRA
	LDH	(.OCPD),A

	WAIT_STAT
	LD	A,#0xb5		; Light gray
	LDH	(.OCPD),A
	LD	A,#0x56
	LDH	(.OCPD),A
	LD	A,#0x4a		; Dark gray
	LDH	(.OCPD),A

	WAIT_STAT
	LD	A,#0x29
	LDH	(.OCPD),A
	XOR	A		; Black
	LDH	(.OCPD),A
	LDH	(.OCPD),A

	RET
