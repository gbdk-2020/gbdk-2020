        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .globl  ___current_metasprite, ___render_shadow_OAM

        .area   _CODE


; void __hide_metasprite(uint8_t id)

___hide_metasprite::
        ldhl    sp, #2
        ld      a, (hl)
        cp      #40
        ret     nc
        
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

        ld      a, e
        cp      #160

        jr      c, 1$

        ret