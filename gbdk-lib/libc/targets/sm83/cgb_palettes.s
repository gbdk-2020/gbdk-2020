	.include	"global.s"

	.title	"CGB support"
	.module	CGB

	.area	_CODE

; void set_sprite_palette(uint8_t first_palette, uint8_t nb_palettes, const palette_color_t *rgb_data) PRESERVES_REGS(b, c);
_set_sprite_palette::		; Non-banked
	ld d, c			; save C
	ld c, #rOCPS
	jr .set_palette

; void set_bkg_palette(uint8_t first_palette, uint8_t nb_palettes, const palette_color_t *rgb_data) PRESERVES_REGS(b, c);
_set_bkg_palette::		; Non-banked
	ld d, c			; save C
	ld c, #rBCPS
	
	; first_palette is in A
	; nb_palettes is in E
	; rgb_data is on the stack
.set_palette::
	add a
	add a
	add a
	or #OCPSF_AUTOINC	; Set auto-increment
	ldh (c), a
	inc c
	
	; E = nb_palettes * 8
	ld a, e
	add a
	add a
	add a
	ld e, a			

	; hl = rgb_data
	ldhl sp, #2
	ld a, (hl+)
	ld h, (hl)
	ld l, a
0$:
	WAIT_STAT
	
	ld a, (hl+)
	ldh (c), a
	dec e
	jr nz, 0$

	ld c, d			; restore C
	pop hl			; get return address
	pop af			; dummy pop
	jp (hl)

; void set_sprite_palette_entry(uint8_t palette, uint8_t entry, uint16_t rgb_data) PRESERVES_REGS(b, c);
_set_sprite_palette_entry::
	ld d, c			; save C
	ld c, #rOCPS
	jr .set_palette_entry

; void set_bkg_palette_entry(uint8_t palette, uint8_t entry, uint16_t rgb_data) PRESERVES_REGS(b, c);
_set_bkg_palette_entry::
	ld d, c			; save C
	ld c, #rBCPS

	; palette is in A
	; entry is in E
	; rgb_data is on the stack
.set_palette_entry::
	; A = palette * 8 + entry * 2
	add a
	add a
	add e
	add a
	or #OCPSF_AUTOINC	; Set auto-increment
	ldh (c), a
	inc c
	
	ldhl sp, #2
	ld a,(hl+)		; rgb_data
	ld e,(hl)

	WAIT_STAT_HL

	ldh (c), a
	ld a, e
	ldh (c), a

	ld c, d			; restore C
	pop hl			; get return address
	pop af			; dummy pop
	jp (hl)
