        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

        .globl ___current_metasprite, ___current_base_tile, ___render_shadow_OAM

        .area   _CODE

; UBYTE __move_metasprite(UINT8 id, UINT8 x, UINT8 y)

___move_metasprite_hflip::
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
        ld      a, (hl+)
        cp      #0x80
        jr      z, 2$
        cpl
        inc     a
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
        xor     #0x40
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

___move_metasprite_vflip::
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
        cpl
        inc     a
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
        xor     #0x20
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

___move_metasprite_hvflip::
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
        ld      a, (hl+)
        cp      #0x80
        jr      z, 2$
        cpl
        inc     a
        add     b
        ld      b, a
        ld      (de), a
        inc     e

        ld      a, (hl+)    ; dx
        cpl
        inc     a
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
        xor     #0x60
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
