	.include	"global.s"

	.PAL_01		= 0x00
	.PAL_23		= 0x01
	.PAL_03		= 0x02
	.PAL_12		= 0x03
	.ATTR_BLK	= 0x04
	.ATTR_LIN	= 0x05
	.ATTR_DIV	= 0x06
	.ATTR_CHR	= 0x07
	.SOUND		= 0x08
	.SOU_TRN	= 0x09
	.PAL_SET	= 0x0A
	.PAL_TRN	= 0x0B
	.ATRC_EN	= 0x0C
	.TEST_EN	= 0x0D
	.ICON_EN	= 0x0E
	.DATA_SND	= 0x0F
	.DATA_TRN	= 0x10
	.MLT_REQ	= 0x11
	.JUMP		= 0x12
	.CHR_TRN	= 0x13
	.PCT_TRN	= 0x14
	.ATTR_TRN	= 0x15
	.ATTR_SET	= 0x16
	.MASK_EN	= 0x17
	.OBJ_TRN	= 0x18

	.area   _CODE

	;; Check if running on SGB
	;;   Set A to 0xFF when running on SGB
	;;   Clear A when running on DMG
.sgb_check::
_sgb_check::
	PUSH	BC
	LD	HL,#.MLT_REQ_4
	CALL	.sgb_transfer
	LD	C,#.P1
	LD	A,#(.P14 | .P15)
	LDH	(C),A
	LDH	A,(C)
	LDH	A,(C)
	LD	E, #4
	LDH	A,(C)		; read delayed
3$:
	LD	B, A

	LD	A,#.P15
	LDH	(C),A
	LDH	A,(C)
	LDH	A,(C)

	LD	A,#.P14
	LDH	(C),A
	LDH	A,(C)
	LDH	A,(C)

	LD	A,#(.P14 | .P15)
	LDH	(C),A
	LDH	A,(C)
	LDH	A,(C)
	LDH	A,(C)
	LDH	A,(C)		; read delayed
	CP	B
	JR	NZ,1$
	
	DEC	E
	JR	NZ,3$
2$:
	LD	E,#0
	POP	BC
	RET
1$:
	LD	HL,#.MLT_REQ_1
	CALL	.sgb_transfer
	LD	E,#1
	POP	BC
	RET

_sgb_transfer::
	LDHL	SP,#2
	LD	A,(HL+)
	LD	H,(HL)
	LD	L,A
	
.sgb_transfer::
	PUSH    BC
	LD	A,(HL)		; Top of command data
	AND	#0x03
	JR	Z,6$
1$:
	PUSH	AF
	LD	C,#.P1
	LDH	(C),A		; Send reset
	LD	A,#(.P14 | .P15)
	LDH	(C),A
	LD	B,#0x10		; Set counter to transfer 16 byte
2$:	LD	E,#0x08		; Set counter to transfer 8 bit
	LD	A,(HL+)
	LD	D,A

3$:
	SRL	D
	LD	A,#.P14		; P14 = high, P15 = low  (output "1")
	JR	C,4$
	LD	A,#.P15		; P14 = low,  P15 = high (output "0")
4$:
	LDH	(C),A
	LDH	A,(C)		; delay
	LDH	A,(C)
	LD	A,#(.P14 | .P15); P14 = high, P15 = high
	LDH	(C),A
	LDH	A,(C)		; delay
	LDH	A,(C)
	DEC	E
	JR	NZ,3$

	DEC	B
	JR	NZ,2$
	
	LD	A,#.P15		; 129th bit "0" output
	LDH	(C),A
	LDH	A,(C)		; delay
	LDH	A,(C)
	LD	A,#(.P14 | .P15)
	LDH	(C),A

	LD	DE,#7000
5$:
	LDH	A,(.P1)		; 3 +
	DEC	DE		; 2 +
	LD	A,D		; 1 +
	OR	E		; 1 +
	JR	NZ,5$		; 3 = 10 cycles

	POP	AF
	DEC	A
	JR	NZ, 1$
6$:
	POP	BC
	RET
	
	
.MLT_REQ_1::
	.byte	((.MLT_REQ << 3) | 1), 0x00
.MLT_REQ_2::
	.byte	((.MLT_REQ << 3) | 1), 0x01
.MLT_REQ_4::
	.byte	((.MLT_REQ << 3) | 1), 0x03
