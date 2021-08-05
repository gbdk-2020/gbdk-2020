        ;; SMS palette routines

        .include        "global.s"

        .title  "CRAM SMS Palettes"
        .module CRAMUtils
        .area   _HOME

; void set_bkg_palette_entry(uint8_t palette, uint8_t entry, uint16_t rgb_data) __z88dk_callee;
_set_bkg_palette_entry::
        pop de
        pop bc
        ld hl, #.VDP_CRAM
        bit 0, c
        jr z, 1$
        set 4, b
1$:
        sla b
        ld c, b
        ld b, #0
        add hl, bc
        pop bc

.write_pal_entry:
        ld a, i
        di
        ld a, l
        out (#.VDP_CMD), a
        ld a, h
        out (#.VDP_CMD), a
        ld a, c
        out (#.VDP_DATA), a
        jr 3$
3$:
        ld a, b
        out (#.VDP_DATA), a
        jp po, 2$
        ei
2$:        
        ld h, d
        ld l, e
        jp (hl)

; void set_sprite_palette_entry(uint8_t palette, uint8_t entry, uint16_t rgb_data) __z88dk_callee;
_set_sprite_palette_entry::
        pop de
        pop bc
        ld hl, #.VDP_CRAM
        ld c, b
        set 4, c
        sla c
        ld b, #0
        add hl, bc
        pop bc

        jp .write_pal_entry

; void set_bkg_palette(uint8_t first_palette, uint8_t nb_palettes, uint16_t *rgb_data) __z88dk_callee;
_set_bkg_palette::
        pop de
        pop bc
        ld hl, #.VDP_CRAM
        bit 0, c
        ld a, b
        ld bc, #0
        jr z, 1$
        ld c, #0x20
1$:
        add hl, bc

        ld c, a
        ld a, i
        di
        ld a, l
        out (#.VDP_CMD), a
        ld a, h
        out (#.VDP_CMD), a
        jp po, 2$
        ei
2$:
        ld a, c
        or a
        jr z, 3$
        
        pop hl
        ld c, #.VDP_DATA
5$:
        ld b, #0x20
4$:        
        outi
        jr nz, 4$
        
        dec a
        jr nz, 5$
3$:        
        ld h, d
        ld l, e
        jp (hl)

; void set_sprite_palette(uint8_t first_palette, uint8_t nb_palettes, uint16_t *rgb_data) __z88dk_callee;
_set_sprite_palette::
        pop de
        pop hl
        ld hl, #(.VDP_CRAM+0x20)

        ld a, i
        di
        ld a, l
        out (#.VDP_CMD), a
        ld a, h
        out (#.VDP_CMD), a
        jp po, 2$
        ei
2$:        
        pop hl
        ld c, #.VDP_DATA

        ld b, #0x20
4$:        
        outi
        jr nz, 4$

        ld h, d
        ld l, e
        jp (hl)

.CRT_DEFAULT_PALETTE::
        .dw 0b0000000000000000
        .dw 0b0000010001000100
        .dw 0b0000100010001000
        .dw 0b0000111111111111
        .dw 0b0000000000001000
        .dw 0b0000000010000000
        .dw 0b0000100000000000
        .dw 0b0000000010001000
        .dw 0b0000100010000000
        .dw 0b0000100000001000
        .dw 0b0000000000001111
        .dw 0b0000000011110000
        .dw 0b0000111100000000
        .dw 0b0000000011111111
        .dw 0b0000111111110000
        .dw 0b0000111100001111
        
