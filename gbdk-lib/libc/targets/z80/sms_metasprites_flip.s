        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .ez80

        .area   _DATA

        .globl ___current_metasprite, ___current_base_tile, ___render_shadow_OAM

        .area   _CODE

.macro MOVE_METASPRITE_BODY neg_dx neg_dy
        ld      iyl, a

        pop     hl
        pop     bc
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
.ifne neg_dy
        neg                     ; apply flipy (or no-op)
.endif
        add     a, a
        sbc     a, a
        ld      h, a
        ld      a, 0(ix)
.ifne neg_dy
        neg                     ; apply flipy (or no-op)
.endif
        ld      l, a
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
.ifne neg_dx
        neg                     ; apply flipx (or no-op)
.endif
        add     a, a
        sbc     a, a
        ld      h, a
        ld      a, 0(ix)
.ifne neg_dx
        neg                     ; apply flipx (or no-op)
.endif
        ld      l, a
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
.endm

___move_metasprite_flipx::
    MOVE_METASPRITE_BODY 1,0

___move_metasprite_flipy::
    MOVE_METASPRITE_BODY 0,1

___move_metasprite_flipxy::
    MOVE_METASPRITE_BODY 1,1
