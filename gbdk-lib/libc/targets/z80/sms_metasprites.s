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

; uint8_t __move_metasprite(uint8_t id, uint8_t x, uint8_t y) __z88dk_callee __preserves_regs(iyh,iyl);

.macro MOVE_METASPRITE_BODY neg_dx neg_dy
        ld      hl, #4
        add     hl, sp

        ld      b, (hl)
        dec     hl
        ld      c, (hl)
        dec     hl
        ld      e, (hl)

        ld      hl, (___current_metasprite)

        ld      a, (___render_shadow_OAM)
        ld      d, a
1$:
        ld      a, (hl)         ; dy
        inc     hl
        cp      #0x80
        jp      z, 2$
.ifne neg_dy
        neg                     ; apply flipy (or no-op)
.endif
        add     b        
        ld      b, a
        cp      #0xD0
        jp      nz, 3$
        ld      a, #0xC0
3$:
        ld      (de), a

        push    de

        ld      a, e
        add     a
        add     #0x40
        ld      e, a

        ld      a, (hl)         ; dx
        inc     hl
.ifne neg_dx
        neg                     ; apply flipx (or no-op)
.endif
        add     c
        ld      c, a
        ld      (de), a
        inc     e

        ld      a, (___current_base_tile)
        add     (hl)            ; tile
        inc     hl
        ld      (de), a

        pop     de
        inc     e

        jp      1$
2$:
        pop     hl
        pop     bc
        inc     sp
        push    hl
        ld      a, e
        sub     c
        ld      l, a
        ret
.endm

___move_metasprite::
    MOVE_METASPRITE_BODY 0,0

___move_metasprite_flipx::
    MOVE_METASPRITE_BODY 1,0

___move_metasprite_flipy::
    MOVE_METASPRITE_BODY 0,1

___move_metasprite_flipxy::
    MOVE_METASPRITE_BODY 1,1
