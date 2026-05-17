	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.globl	_set_sprite_palette, _set_bkg_palette

	.area	_CODE
        
_cgb_compatibility::
_set_default_palette::
	ld bc, #1$
	xor a
	ld e, #1
	push bc
	call _set_sprite_palette	; preserve BC
	
	xor a
	ld e, #1
	push bc
	call _set_bkg_palette
	ret
1$:
	.DW	0x7FFF, 0x56B5, 0x294A, 0x0000
