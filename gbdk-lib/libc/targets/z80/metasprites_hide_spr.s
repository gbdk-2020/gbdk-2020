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
        cp  #64
        ret nc

        sub e
        ret c
        ret z

        ld c, a
        xor a
        ld b, a

        ld hl, #___render_shadow_OAM
        ld h, (hl)
        ld l, e

        ld d, h
        inc e
        ld (hl), #0xC0

        dec c
        ret z

        ldir
        ret
