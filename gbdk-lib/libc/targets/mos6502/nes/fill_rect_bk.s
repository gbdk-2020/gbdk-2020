    .include    "global.s"

    .area   OSEG (PAG, OVR)
    _fill_bkg_rect_PARM_3::     .ds 1
    _fill_bkg_rect_PARM_4::     .ds 1
    _fill_bkg_rect_PARM_5::     .ds 1
    .xpos:                      .ds 1
    .ypos:                      .ds 1

    .area   _HOME

_fill_bkg_rect::
    ; TODO: Switch to use vram transfer buffer when screen not blanked
    .define .width  "_fill_bkg_rect_PARM_3"
    .define .height "_fill_bkg_rect_PARM_4"
    .define .tile   "_fill_bkg_rect_PARM_5"
    sta *.xpos
    stx *.ypos
    ldy *.height
1$:
    lda #0
    sta *.tmp+1
    lda *.ypos
    asl
    rol *.tmp+1
    asl
    rol *.tmp+1
    asl
    rol *.tmp+1
    asl
    rol *.tmp+1
    asl
    rol *.tmp+1
    ora *.xpos
    sta *.tmp
    ;
    lda *.tmp+1
    ora #0x20
    sta PPUADDR
    lda *.tmp
    sta PPUADDR
    ldx *.width
    lda *.tile
2$:
    sta PPUDATA
    dex
    bne 2$
    inc *.ypos
    dey
    bne 1$
    rts
