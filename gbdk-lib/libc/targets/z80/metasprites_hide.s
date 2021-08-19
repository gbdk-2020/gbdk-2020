        .include    "global.s"

        .title  "Metasprites"
        .module Metasprites

        .globl  ___current_metasprite, ___render_shadow_OAM

        .area   _CODE


; void __hide_metasprite(uint8_t id) __z88dk_fastcall __preserves_regs(iyh,iyl);

___hide_metasprite::
        ld      e, a

        ld      hl, (___current_metasprite)

        ld      bc, #3

        ld      a, (___render_shadow_OAM)
        ld      d, a
1$:
        ld      a, (hl)
        cp      #0x80
        ret     z

        ld      a, #0x40
        cp      e
        ret     c

        add     hl, bc

        ld      a, #0xC0
        ld      (de), a

        inc     e
        jp      1$
