        ;; SMS palette routines

        .include        "global.s"

        .title  "CRAM SMS Palettes"
        .module CRAMUtils
        .area   _HOME

; void set_palette_entry(uint8_t palette, uint8_t entry, uint16_t rgb_data) __z88dk_callee;
_set_palette_entry::
        pop de
        pop bc
        ld hl, #.VDP_CRAM
        bit 0, c
        jr z, 1$
        set 4, b
1$:
        ld c, b
        ld b, #0
        add hl, bc
        pop bc

        ld a, i
        di
        ld a, l
        out (#.VDP_CMD), a
        ld a, h
        out (#.VDP_CMD), a
        ld a, c
        jp po, 2$
        ei
2$:
        out (#.VDP_DATA), a
        
        ld h, d
        ld l, e
        jp (hl)

; void set_palette(uint8_t first_palette, uint8_t nb_palettes, uint16_t *rgb_data) __z88dk_callee;
_set_palette::
        pop de
        pop bc

        DISABLE_VBLANK_COPY             ; switch OFF copy shadow SAT

        ld hl, #.VDP_CRAM
        bit 0, c
        ld a, b
        ld bc, #0
        jr z, 1$
        ld c, #0x10
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
        ld b, #0x10
4$:        
        outi
        jr nz, 4$
        
        dec a
        jr nz, 5$
3$:        
        ENABLE_VBLANK_COPY              ; switch ON copy shadow SAT

        ld h, d
        ld l, e
        jp (hl)

.CRT_DEFAULT_PALETTE::
        .db 0b00111111
        .db 0b00101010
        .db 0b00010101
        .db 0b00000000
        .db 0b00000010
        .db 0b00001000
        .db 0b00100000
        .db 0b00001010
        .db 0b00101000
        .db 0b00100010
        .db 0b00000011
        .db 0b00001100
        .db 0b00110000
        .db 0b00001111
        .db 0b00111100
        .db 0b00110011
        
