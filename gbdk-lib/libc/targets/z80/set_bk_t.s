        .include        "global.s"

        .globl  .set_xy_btt
        ;; BANKED:	checked
        .area   _HOME

; void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles) __z88dk_callee __preserves_regs(iyh, iyl);
_set_bkg_tiles::

        pop hl          ; HL = ret
        pop bc          ; BC = YX
        pop de          ; DE = WH
        ex (sp), hl     ; HL = data
        ex de, hl       ; HL = WH, DE = data
        ld a, b
        ld b, d
        ld d, a
        ld a, c
        ld c, e
        ld e, a         ; BC = data, DE = YX

        call .set_xy_btt

        ret