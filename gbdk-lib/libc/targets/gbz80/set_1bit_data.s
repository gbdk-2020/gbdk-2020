	.include	"global.s"

	.area	_HOME

_set_bkg_1bit_data::
_set_win_1bit_data::
	ld d, #0x90
	ldh a, (.LCDC)
	and #LCDCF_BG8000
	jr z, .copy_1bit_tiles
_set_sprite_1bit_data::
	ld d, #0x80
.copy_1bit_tiles:
	push bc

	lda hl, 8(sp)
	ld a, (hl)
	push af

	lda hl, 6(sp)
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
	add d ; Add base offset of target tile "block"
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
	ld b, #8
3$:
	pop af
	push af

	dec a
	jr z, 4$
	dec a
	jr z, 5$

	WAIT_STAT
	ld a, (hl)
	ld (de), a
	inc de
	WAIT_STAT
	ld a, (hl+)
	ld (de), a
	inc de
	jr 6$
4$:
	WAIT_STAT
	ld a, (hl+)
	ld (de), a
	inc de
	WAIT_STAT
	xor a
	ld (de), a
	inc de
	jr 6$
5$:
	WAIT_STAT
	xor a
	ld (de), a
	inc de
	WAIT_STAT
	ld a, (hl+)
	ld (de), a
	inc de
6$:	
	dec b
	jr nz, 3$

	dec c
	jr nz, 1$

	pop af

	pop bc
	ret