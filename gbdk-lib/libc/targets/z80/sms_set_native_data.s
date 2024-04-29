        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_sprite_4bpp_data(uint8_t start, uint16_t ntiles, const void *src) __preserves_regs(iyh,iyl);
_set_sprite_4bpp_data::
; void set_sprite_native_data(uint8_t start, uint16_t ntiles, const void *src) __preserves_regs(iyh,iyl);
_set_sprite_native_data::
	ld h, #1
        ld l, a

; void set_bkg_native_data(uint16_t start, uint16_t ntiles, const void *src) __preserves_regs(iyh,iyl);
_set_bkg_native_data::
; void set_bkg_4bpp_data(uint16_t start, uint16_t ntiles, const void *src) __preserves_regs(iyh,iyl);
_set_bkg_4bpp_data::
; void set_native_tile_data(uint16_t start, uint16_t ntiles, const void *src) __preserves_regs(iyh,iyl);
_set_native_tile_data::
        ld b, h
        ld c, l

        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

        ld h, b
        ld l, c

        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl

        ld bc, #.VDP_VRAM
        add hl, bc

        VDP_WRITE_CMD h, l

        pop hl
        ex (sp), hl

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
        jp  nz, 1$

        dec d
        jp nz, 1$

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        ret
