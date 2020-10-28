	.include	"global.s"

	.globl	.copy_vram

	.area	_BASE

_set_bkg_1bit_data::
_set_win_1bit_data::
	ld d, #0x90
	ldh a, (.LCDC)
	bit 4, a
	jr z, .copy_1bit_tiles
_set_sprite_1bit_data::
	ld d, #0x80
.copy_1bit_tiles:
	push bc
	lda hl, 4(sp)
	ld a, (hl+) ; ID of 1st tile
	ld e, a
	ld a, (hl+) ; Nb of tiles
	ld c, a
	ld a, (hl+) ; Src ptr
	ld h, (hl)
	ld l, a
	
	; Compute dest ptr
	swap e ; *16 (size of a tile)
	ld a, e
	and #0x0F ; Get high bits
	add a, d ; Add base offset of target tile "block"
	ld d, a
	ld a, e
	and #0xF0 ; Get low bits only
	ld e, a
1$:
	; Wrap from past $97FF to $8800 onwards
	; This can be reduced to "bit 4 must be clear if bit 3 is set"
	bit 3, d
	jr z, 2$
	res 4, d
2$:
	.rept 3		; Repeat this 3 times to copy 12 bytes...
		WAIT_STAT
		; We have 16 cycles free to access VRAM, during which we can expand 2 bytes into 4 bytes
		ld a, (hl+)
		ld (de), a
		inc e ; inc de
		ld (de), a
		inc e ; inc de
		ld a, (hl+)
		ld (de), a
		inc e ; inc de
		ld (de), a
		inc e ; inc de
	.endm
	WAIT_STAT
	; We have 16 cycles free to access VRAM, during which we can expand 2 bytes into 4 byte
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld (de), a
	inc de ; This actually can overflow into the high byte

	dec c
	jr nz, 1$
	
	pop bc
	ret