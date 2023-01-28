        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .area   _DATA

___current_metasprite::
        .ds     0x02
___current_base_tile::
        .ds     0x01
___current_base_prop::
	.ds     0x01

        .area   _INITIALIZED
___render_shadow_OAM::
        .ds     0x01

        .area   _INITIALIZER
        .db     #>_shadow_OAM

        .area   _CODE

; uint8_t __move_metasprite(uint8_t id, uint16_t yx)
; a: id
; de: yx

___move_metasprite::
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
