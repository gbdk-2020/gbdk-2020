        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _HOME

; void set_sprite_1bpp_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee;
_set_sprite_1bpp_data::
        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

        pop af
        pop hl
        pop iy
        pop de
        push af

        add hl, hl
        add hl, hl
        add hl, hl

        ld bc, #.VDP_SPRDATA0
        add hl, bc


        VDP_WRITE_CMD h, l

        ex de, hl

        ld d, iyh
        ld e, iyl

        ld c, #.VDP_DATA
        inc d
        inc e
        jr 2$
1$:
        ld b, #8
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
