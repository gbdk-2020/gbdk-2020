        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .globl  ___render_shadow_OAM

        .area   _CODE

; void hide_sprites_range(UINT8 from, UINT8 to) __z88dk_callee __preserves_regs(iyh,iyl);

_hide_sprites_range::
        pop hl
        pop de
        push hl

        ld  a, d
        cp  #(32+1)
        ret nc

        sub e
        ret c
        ret z
        ld b, a

        ld a, e
        add a
        add a
        ld l, a

        ld a, (___render_shadow_OAM)
        ld h, a
        ld de, #4
1$:
        ld (hl), #0xC0
        add hl, de
        djnz 1$

        ret
