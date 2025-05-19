        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .ez80

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

; uint8_t __move_metasprite(uint8_t id, int16_t x, int16_t y);
; a      == id
; de     == x
; (sp+2) == y
___move_metasprite::
        ld      iyl, a

        pop     hl
        pop     bc              ; de = x, bc = y
        push    hl

        push    af
        push    ix

        ld      ix, (___current_metasprite)

        ld      a, (___render_shadow_OAM)
        ld      iyh, a
1$:
        ld      a, 0(ix)        ; dy
        cp      #0x80
        jp      z, 2$

        add     a, a
        sbc     a, a
        ld      h, a
        ld      l, 0(ix)
        inc     ix
        add     hl, bc
        ld      b, h
        ld      c, l

        ld      a, #0x20
        add     l
        ld      l, a
        adc     h
        sub     l
        jp      nz, 5$
        ld      a, l
        cp      #(0x20 + 0xC0)
        jp      c, 6$
5$:
        ld      0(iy), #0xC0
        jp      7$
6$:
        ld      0(iy), c
7$:
        ld      a, 0(ix)        ; dx
        add     a, a
        sbc     a, a
        ld      h, a
        ld      l, 0(ix)
        inc     ix
        add     hl, de
        ex      de, hl
        ld      a, d
        or      a
        jp      nz, 4$

        ld      a, iyl
        rlca
        ld      iyl, a

        ld      0x40(iy), e

        ld      a, (___current_base_tile)
        add     0(ix)           ; tile
        inc     ix
        ld      0x41(iy), a

        ld      a, iyl
        rrca
        inc     a
        ld      iyl, a

        jp      1$
4$:
        ld      0(iy), #0xC0
        inc     ix
        inc     iyl
        jp      1$
2$:
        pop     ix
        pop     bc

        ld      a, iyl
        sub     b
        ret
