    .include    "global.s"

    .area   OSEG (PAG, OVR)
    _set_bkg_attributes_PARM_3::    .ds 1
    _set_bkg_attributes_PARM_4::    .ds 1
    _set_bkg_attributes_PARM_5::    .ds 2
    .xpos:                          .ds 1
    .ypos:                          .ds 1
    .num_rows:                      .ds 1
    .src_tiles:                     .ds 2

    .area   _HOME

_set_bkg_attributes::
    ; TODO: Switch to use vram transfer buffer when screen not blanked
    .define .width  "_set_bkg_attributes_PARM_3"
    .define .height "_set_bkg_attributes_PARM_4"
    .define .tiles  "_set_bkg_attributes_PARM_5"
    lsr         ; Only support even-numbered attribute coordinates for now.
    sta *.xpos
    stx *.ypos
    lsr *.ypos  ; Only support even-numbered attribute coordinates for now.
    lda .tiles
    sta *.src_tiles
    lda .tiles+1
    sta *.src_tiles+1
    lda *.height
    lsr         ; Only support even-numbered attribute dimensions for now.
    sta *.num_rows
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
    ora *.xpos
    ora #0xC0
    sta *.tmp
    ;
    lda *.tmp+1
    ora #0x23
    sta PPUADDR
    lda *.tmp
    sta PPUADDR
    lda *.width
    lsr         ; Only support even-numbered attribute dimensions for now.
    tax
    ldy #0
2$:
    lda [*.src_tiles],y
    iny
    sta PPUDATA
    dex
    bne 2$
    ; .src_tiles += y
    tya
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
    inc *.ypos
    dec *.num_rows
    bne 1$
    rts
