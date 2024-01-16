        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _HOME

; void set_tile_1bpp_data(uint16_t start, uint16_t ntiles, const void *src, uint16_t colors) __z88dk_callee;
_set_tile_1bpp_data::
        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

        pop af
        pop hl
        pop iy
        pop de
        pop bc
        push af

        push ix
        ld ixl, c

        add hl, hl
        add hl, hl
        add hl, hl

        push hl

        ld bc, #.VDP_COLORDATA0
        add hl, bc
        ex (sp), hl

        ld bc, #.VDP_TILEDATA0
        add hl, bc

        ld c, #.VDP_DATA
        inc iyh
        inc iyl
        jr 2$

1$:
        VDP_WRITE_CMD h, l
        ex de, hl
        ld b, #8
3$:        
        outi
        jr nz, 3$
        ex de, hl

        ld a, #8
        ADD_A_REG16 h, l
        ex (sp), hl

        VDP_WRITE_CMD h, l
        ld b, #8
        ld a, ixl
4$:        
        out (c), a
        inc hl
        dec b
        jr nz, 4$

        ex (sp), hl
2$:
        dec iyl
        jr  nz, 1$

        dec iyh
        jr nz, 1$

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        pop hl
        pop ix

        ret
