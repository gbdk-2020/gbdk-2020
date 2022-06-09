    .include    "global.s"

    .title  "Metasprites"
    .module Metasprites

    .globl  ___current_metasprite, ___render_shadow_OAM

    .area   _HOME

; void __hide_metasprite(uint8_t id)

___hide_metasprite::
    asl
    asl
    tax
    ldy #0
___hide_metasprite_loop:
    lda [*___current_metasprite],y
    cmp #0x80
    beq ___hide_metasprite_end
    iny
    iny
    iny
    iny
    lda #240
    sta _shadow_OAM+OAM_POS_Y,x
    inx
    inx
    inx
    inx
    bne ___hide_metasprite_loop
___hide_metasprite_end:
    rts
