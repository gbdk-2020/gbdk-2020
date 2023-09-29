    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
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
    ; Prefer vertical stripes if height > width
    cpy *.width
    beq _fill_bkg_rect_horizontalStripes
    bcs _fill_bkg_rect_verticalStripes
_fill_bkg_rect_horizontalStripes:
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
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_horizontal
    ldx *.width
2$:
    lda *.tile
    jsr .ppu_stripe_write_byte
    dex
    bne 2$
    jsr .ppu_stripe_end
    inc *.ypos
    dey
    bne 1$
    rts

_fill_bkg_rect_verticalStripes:
    ldy *.width
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
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_vertical
    ldx *.height
2$:
    lda *.tile
    jsr .ppu_stripe_write_byte
    dex
    bne 2$
    jsr .ppu_stripe_end
    inc *.xpos
    dey
    bne 1$
    rts
