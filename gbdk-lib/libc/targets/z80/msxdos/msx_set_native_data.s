        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .ez80
        
        .area   _HOME

; void set_native_tile_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee
_set_native_tile_data::
        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

        pop bc
        pop hl
        pop iy
        pop de
        push bc

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
        ex de, hl
        ld b, #8
4$:        
        outi
        jr nz, 4$
        ex de, hl

        ld a, #8
        ADD_A_REG16 h, l
        ex (sp), hl

2$:
        dec iyl
        jr  nz, 1$

        dec iyh
        jr nz, 1$

        pop hl

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        ret
