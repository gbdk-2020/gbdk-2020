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
        sla b
        ld c, b
        ld b, #0
        add hl, bc
        pop bc

        ld a, l
        di
        out (#.VDP_CMD), a
        ld a, h
        out (#.VDP_CMD), a
        ld a, c
        out (#.VDP_DATA), a
        jr 3$
3$:
        ld a, b
        ei
        out (#.VDP_DATA), a

        ex de, hl
        jp (hl)

; void set_palette(uint8_t first_palette, uint8_t nb_palettes, uint16_t *rgb_data) __z88dk_callee;
_set_palette::
        pop de
        pop bc

        DISABLE_VBLANK_COPY              ; switch OFF copy shadow SAT

        ld hl, #.VDP_CRAM
        bit 0, c
        ld a, b
        ld bc, #0
        jr z, 1$
        ld c, #0x20
1$:
        add hl, bc
        
        ld c, a

        ld a, l
        di
        out (#.VDP_CMD), a
        ld a, h
        ei
        out (#.VDP_CMD), a

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
        ENABLE_VBLANK_COPY               ; switch ON copy shadow SAT

        ex de, hl
        jp (hl)
