	.include	"global.s"

	.globl	.copy_vram

	;; BANKED:	checked
	.area	_BASE

_set_bkg_data::
_set_win_data::
	ld d, #0x90
	ldh a, (.LCDC)
	bit 4, a
	jr z, .copy_tiles
_set_sprite_data::
	ld d, #0x80
.copy_tiles:
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
	
copy_tile$:
	; Wrap from past $97FF to $8800 onwards
	; This can be reduced to "bit 4 must be clear if bit 3 is set"
	bit 3, d
	jr z, 1$
	res 4, d
1$:
	ldh a, (.STAT)
	and #2 ; Check if in LCD modes 0 or 1
	jr nz, 1$
	; We have 16 cycles free to access VRAM, during which we can copy 3 bytes
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	; Repeat this 4 more times to copy 15 of the 16 bytes...
2$:
	ldh a, (.STAT)
	and #2
	jr nz, 2$
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
3$:
	ldh a, (.STAT)
	and #2
	jr nz, 3$
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
4$:
	ldh a, (.STAT)
	and #2
	jr nz, 4$
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
5$:
	ldh a, (.STAT)
	and #2
	jr nz, 5$
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	ld a, (hl+)
	ld (de), a
	inc e ; inc de
	; Now, copy the final byte, wherein we will increment `de` in full
6$:
	ldh a, (.STAT)
	and #2
	jr nz, 6$
	ld a, (hl+)
	ld (de), a
	inc de ; This actually can overflow into the high byte
	dec c
	jr nz, copy_tile$
	
	pop bc
	ret