        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

___current_metasprite:: 
        .ds 0x02
___current_base_tile::
        .ds 0x01
___render_shadow_OAM::
        .ds     0x01

        .area   _GSINIT

        ld      a, #>_shadow_OAM
        ld      (___render_shadow_OAM), a 

        .area   _CODE

; UBYTE __move_metasprite(UINT8 id, UINT8 x, UINT8 y)

___move_metasprite::
        ldhl    sp, #4
        ld      a, (hl-)
        ld      b, a
        ld      a, (hl-)
        ld      c, a
        ld      a, (hl-)
        add     a
        add     a
        ld      e, a

        ld      hl, #___current_metasprite
        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

        ld      a, (___render_shadow_OAM)
        ld      d, a
1$:
        ld      a, (hl+)    ; dy
        cp      #0x80
        jr      z, 2$
        add     b
        ld      b, a
        ld      (de), a
        inc     e

        ld      a, (hl+)    ; dx
        add     c
        ld      c, a
        ld      (de), a
        inc     e

        ld      a, (___current_base_tile)
        add     (hl)        ; tile
        inc     hl
        ld      (de), a
        inc     e

        ld      a, (hl+)    ; props
        ld      (de), a
        inc     e

        jr      1$
2$:
        ldhl    sp, #2
        ld      a, e
        srl     a
        srl     a
        sub     (hl)
        ld      e, a

        ret

; void __hide_metasprite(UINT8 id)

___hide_metasprite::
        ldhl    sp, #2
        ld      a, (hl-)
        add     a
        add     a
        ld      e, a

        ld      hl, #___current_metasprite
        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

        ld      bc, #3

        ld      a, (___render_shadow_OAM)
        ld      d, a
1$:
        ld      a, (hl+)
        cp      #0x80
        ret     z

        add     hl, bc

        xor     a
        ld      (de), a

        inc     e
        inc     e
        inc     e
        inc     e
        jr      1$
