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
        push    hl

        inc     hl 

        ld      a, (___render_shadow_OAM)
        ld      d, a

        ld      a, (hl+)
        inc     a
1$:
        dec     a
        add     a
        add     a
        add     e
        ld      e, a

        ld      a, (hl+)    ; dy
        add     b
        ld      (de), a
        inc     e

        ld      a, (hl+)    ; dx
        add     c
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

        ld      a, (hl+)
        or      a
        jr      nz, 1$

        pop     hl
        ld      e,(hl)

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
        inc     hl 

        ld      bc, #4

        ld      a, (___render_shadow_OAM)
        ld      d, a

        ld      a, (hl+)
        inc     a
1$:
        dec     a
        add     a
        add     a
        add     e
        ld      e, a

        xor     a
        ld      (de), a

        inc     e
        inc     e
        inc     e
        inc     e
        add     hl, bc
       
        ld      a, (hl+)
        or      a
        jr      nz, 1$

        ret
