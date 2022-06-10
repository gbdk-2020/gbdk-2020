    .include    "global.s"

    .title  "Metasprites"
    .module Metasprites

    .globl  ___render_shadow_OAM

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
