	.include        "global.s"

	.area _BASE

.drawing_vbl::
	LDH	A,(.LCDC)
	OR	#LCDCF_BG8000	; Set BG Chr to 0x8000
	LDH	(.LCDC),A

	LD	A,#72		; Set line at which LCD interrupt occurs
	LDH	(.LYC),A

	RET

	;; Is the STAT check required, as we are already in the HBL?
.drawing_lcd::
	WAIT_STAT

	LDH	A,(.LCDC)
	AND	#~LCDCF_BG8000  ; Set BG Chr to 0x8800
	LDH	(.LCDC),A

	RET
