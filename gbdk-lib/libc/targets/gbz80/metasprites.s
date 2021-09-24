        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

___current_metasprite:: 
        .ds     0x02
___current_base_tile::
        .ds     0x01

        .area   _INITIALIZED
___render_shadow_OAM::
        .ds     0x01
        
        .area   _INITIALIZER
        .db     #>_shadow_OAM

        .area   _CODE

; uint8_t __move_metasprite(uint8_t id, uint8_t x, uint8_t y)

___move_metasprite::
        ldhl    sp, #4
        ld      a, (hl-)
        ld      b, a
        ld      a, (hl-)
        ld      c, a
        ld      a, (hl)
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
