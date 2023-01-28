        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

        .globl ___current_metasprite, ___current_base_tile, ___current_base_prop, ___render_shadow_OAM

        .area   _CODE

; uint8_t __move_metasprite(uint8_t id, uint16_t yx)
; a: id
; de: yx

___move_metasprite_flipy::
___move_metasprite_hflip::
        cp      #40
        jr      c, 0$
        xor     a
        ret
0$:
        push    af
        add     a
        add     a
        ld      c, a

        ld      hl, #___current_metasprite
        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

        ld      a, (___render_shadow_OAM)
        ld      b, a
1$:
        ld      a, (hl+)
        cp      #0x80
        jr      z, 2$
        cpl
        inc     a
        add     d
        ld      d, a
        ld      (bc), a
        inc     c

        ld      a, (hl+)    ; dx
        add     e
        ld      e, a
        ld      (bc), a
        inc     c

        ld      a, (___current_base_tile)
        add     (hl)        ; tile
        inc     hl
        ld      (bc), a
        inc     c

	ld      a, (___current_base_prop)
        add     (hl)        ; props
        inc     hl
        xor     #0x40
        ld      (bc), a
        inc     c

        ld      a, c
        cp      #160
        jr      c, 1$
2$:
        ld      a, c
        srl     a
        srl     a
        pop     de
        sub     d

        ret

___move_metasprite_flipx::
___move_metasprite_vflip::
        cp      #40
        jr      c, 0$
        xor     a
        ret
0$:
        push    af
        add     a
        add     a
        ld      c, a

        ld      hl, #___current_metasprite
        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

        ld      a, (___render_shadow_OAM)
        ld      b, a
1$:
        ld      a, (hl+)    ; dy
        cp      #0x80
        jr      z, 2$
        add     d
        ld      d, a
        ld      (bc), a
        inc     c

        ld      a, (hl+)    ; dx
        cpl
        inc     a
        add     e
        ld      e, a
        ld      (bc), a
        inc     c

        ld      a, (___current_base_tile)
        add     (hl)        ; tile
        inc     hl
        ld      (bc), a
        inc     c

	ld      a, (___current_base_prop)
        add     (hl)        ; props
        inc     hl
        xor     #0x20
        ld      (bc), a
        inc     c

        ld      a, c
        cp      #160
        jr      c, 1$
2$:
        ld      a, c
        srl     a
        srl     a
        pop     de
        sub     d

        ret

___move_metasprite_flipxy::
___move_metasprite_hvflip::
        cp      #40
        jr      c, 0$
        xor     a
        ret
0$:
        push    af
        add     a
        add     a
        ld      c, a

        ld      hl, #___current_metasprite
        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

        ld      a, (___render_shadow_OAM)
        ld      b, a
1$:
        ld      a, (hl+)
        cp      #0x80
        jr      z, 2$
        cpl
        inc     a
        add     d
        ld      d, a
        ld      (bc), a
        inc     c

        ld      a, (hl+)    ; dx
        cpl
        inc     a
        add     e
        ld      e, a
        ld      (bc), a
        inc     c

        ld      a, (___current_base_tile)
        add     (hl)        ; tile
        inc     hl
        ld      (bc), a
        inc     c

	ld      a, (___current_base_prop)
        add     (hl)        ; props
        inc     hl
        xor     #0x60
        ld      (bc), a
        inc     c

        ld      a, c
        cp      #160
        jr      c, 1$
2$:
        ld      a, c
        srl     a
        srl     a
        pop     de
        sub     d

        ret
