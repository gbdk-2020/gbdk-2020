	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.globl	_set_sprite_palette, _set_bkg_palette

	.area	_CODE
        
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
