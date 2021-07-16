        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .globl  ___render_shadow_OAM

        .area   _CODE

; void hide_sprites_range(UINT8 from, UINT8 to)

_hide_sprites_range::
        ldhl    sp, #2
        ld      a, (hl+)

        add     a
        add     a
        ld      e, a

        ld      a, (hl-)
        sub     (hl)

        ret     c
        ret     z

        ld      hl, #___render_shadow_OAM
        ld      h, (hl)
        ld      l, e

        ld      de, #4

        rra                     ; carry is never set here, because of ret c above
        jr      nc, 0$

        ld      (hl), d
        add     hl, de

        ret     z               ; z is not affected by 16-bit add

0$:
        rra                     ; carry is never set here, because of ret c above
        jr      nc, 1$

        ld      (hl), d
        add     hl, de

        ld      (hl), d
        add     hl, de

        ret     z               ; z is not affected by 16-bit add

1$:
        ld      (hl), d
        add     hl, de

        ld      (hl), d
        add     hl, de

        ld      (hl), d
        add     hl, de

        ld      (hl), d
        add     hl, de

        dec     a
        jr      nz, 1$

        ret
