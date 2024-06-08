        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .area   _HOME

; translate coords in DE into address in DE
.coords_to_address::
        ld a, (_shadow_VDP_R2)
        rlca
        rlca
        and #0b01111000
        ld b, a

        ld a, d
        add #.SCREEN_Y_OFS
        ld d, a
        xor a
        ld c, a
        FAST_MOD8 d #28
        ld d, a

        ld a, e
        add #.SCREEN_X_OFS
        and #0x1f
        ld e, a

        ld a, d
        rrca                    ; rrca(2) == rlca(6)
        rrca
        ld d, a
        and #0x07
        add b
        ld b, a
        ld a, #0xC0
        and d
        sla e
        add e
        ld e, a
        ld d, b                 ; dest DE = BC + ((0x20 * Y) * 2) + (X * 2)

        ret

; uint8_t * get_bkg_xy_addr(uint8_t x, uint8_t y) __z88dk_callee __preserves_regs(iyh, iyl);

_get_bkg_xy_addr::
        pop hl
        ex (sp), hl
        ex de, hl
        call .coords_to_address
        ex de, hl
        ret