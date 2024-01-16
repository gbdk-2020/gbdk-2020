        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_native_tile_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
_set_native_tile_data::
        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

        pop de          ; pop ret address
        pop hl

        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl

        ld bc, #.VDP_VRAM
        add hl, bc

        VDP_WRITE_CMD h, l

        pop bc
        pop hl
        push de

        ld e, c
        ld d, b
        
        ld c, #.VDP_DATA
        inc d
        inc e
        jr 2$

1$:
        ld b, #32
3$:        
        outi
        jr nz, 3$
2$:
        dec e
        jr  nz, 1$

        dec d
        jr nz, 1$
        
        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        ret
