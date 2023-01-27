    .include    "global.s"

    .title  "Metasprites"
    .module Metasprites

    .area   _HOME

    .area   _INITIALIZED
    ___render_shadow_OAM:: .ds     0x01

    .area   _INITIALIZER
    .db     #>_shadow_OAM

.define id    ".tmp+1"

.move_metasprite_prologue::
    sta *id
    asl
    asl
    tax
    ; Make 8x16 mode bit fetch from second pattern table by setting bit0 of tile index
    lda *_shadow_PPUCTRL
    lsr
    lsr
    lsr
    lsr
    lsr
    and #1
    ora *___current_base_tile
    sta *___current_base_tile
    rts

.move_metasprite_epilogue::
    ; Return number of hardware sprites used
    txa
    lsr
    lsr
    sec
    sbc *id
    rts
