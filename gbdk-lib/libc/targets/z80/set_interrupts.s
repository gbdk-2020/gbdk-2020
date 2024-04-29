        .include        "global.s"

        .title  "Set interrupts"
        .module SetInterrupts

        .globl _shadow_VDP_R0
        .globl _shadow_VDP_R1

        .area   _HOME

_set_interrupts::
        di

        ld a, (_shadow_VDP_R1)
        bit 0, l
        jr z, 1$
        or #.R1_IE
        jr 2$
1$:
        and #~.R1_IE
2$:
        ld (_shadow_VDP_R1), a
        out (.VDP_CMD), a
        ld a, #.VDP_R1
        out (.VDP_CMD), a

        ld a, (_shadow_VDP_R0)
        bit 1, l
        jr z, 3$
        or #.R0_IE1
        jr 4$
3$:
        and #~.R0_IE1
4$:
        ld (_shadow_VDP_R0), a
        out (.VDP_CMD), a
        ld a, #.VDP_R0
        out (.VDP_CMD), a

        ei
        ret
