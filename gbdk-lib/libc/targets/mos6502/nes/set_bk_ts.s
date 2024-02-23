    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_tiles_PARM_3::     .ds 1
    _set_bkg_tiles_PARM_4::     .ds 1
    _set_bkg_tiles_PARM_5::     .ds 2
    .xpos:                      .ds 1
    .ypos:                      .ds 1
    .num_rows:                  .ds 1
    .src_tiles:                 .ds 2

    .area   _ZP
    __map_tile_offset::         .ds 1

    .area   _HOME

_set_bkg_tiles::
    .define .width  "_set_bkg_tiles_PARM_3"
    .define .height "_set_bkg_tiles_PARM_4"
    .define .tiles  "_set_bkg_tiles_PARM_5"
    sta *.xpos
    stx *.ypos
    lda .tiles
    sta *.src_tiles
    lda .tiles+1
    sta *.src_tiles+1
    lda *.height
    sta *.num_rows
    ; Prefer vertical stripes if height > width
    cmp *.width
    beq _set_bkg_tiles_horizontalStripes
    bcs _set_bkg_tiles_verticalStripes
_set_bkg_tiles_horizontalStripes:
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
    ldy #0
2$:
    lda [*.src_tiles],y
    clc
    adc *__map_tile_offset
    iny
    jsr .ppu_stripe_write_byte
    dex
    bne 2$
    jsr .ppu_stripe_end
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

.define .num_cols  ".num_rows"

_set_bkg_tiles_verticalStripes::
    lda *.width
    sta *.num_cols
    ldy #0
1$:
    lda *.tiles
    sta *.src_tiles
    lda *.tiles+1
    sta *.src_tiles+1
    ;
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
    lda [*.src_tiles],y
    clc
    adc *__map_tile_offset
    jsr .ppu_stripe_write_byte
    ; .src_tiles += width
    lda *.width
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
    dex
    bne 2$
    jsr .ppu_stripe_end
    iny
    inc *.xpos
    dec *.num_cols
    bne 1$
    rts
