    .include    "global.s"

    .title  "Sprites"
    .module Sprites

    .globl  ___render_shadow_OAM

    .area   _INITIALIZED
    ___render_shadow_OAM:: .ds     0x01

    .area   _INITIALIZER
    .db     #>_shadow_OAM

    .area   _HOME

; void hide_sprites_range(UINT8 from, UINT8 to)

_hide_sprites_range::
    pha
    txa
    asl
    asl
    sta *.tmp
    pla
    asl
    asl
    tax
    lda #240
_hide_sprites_range_loop:
    sta _shadow_OAM+OAM_POS_Y,x
    inx
    inx
    inx
    inx
    cpx *.tmp
    bne _hide_sprites_range_loop
    rts
