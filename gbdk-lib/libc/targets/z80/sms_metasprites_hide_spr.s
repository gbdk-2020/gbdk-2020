        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .globl  ___render_shadow_OAM

        .area   _CODE

; void hide_sprites_range(UINT8 from, UINT8 to);

_hide_sprites_range::
        ld e, a

        ld a, l
        cp #(64+1)
        ret nc

        sub e
        ret c
        ret z

        ld c, a
        ld b, #0

        ld hl, #___render_shadow_OAM
        ld h, (hl)
        ld l, e

        ld (hl), #0xC0

        dec c
        ret z

        ld d, h
        inc e

        ldir
        ret
