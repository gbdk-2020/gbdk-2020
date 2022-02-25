        .include        "global.s"

        .globl  .fill_rect_xy

        .area   _HOME

; void fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t tile) __z88dk_callee __preserves_regs(iyh, iyl);
_fill_rect::
        pop hl          ; HL = ret
        pop bc          ; BC = YX
        pop de          ; DE = WH
        ex (sp), hl     ; HL = data
        ex de, hl       ; HL = WH, DE = data

        xor a
        FAST_MOD8 b #.VDP_MAP_HEIGHT

        ld b, d
        ld d, a

        ld a, c
        ld c, e
        ld e, a         ; BC = data, DE = YX

        jp .fill_rect_xy
