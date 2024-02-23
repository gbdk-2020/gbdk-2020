    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_submap_PARM_3::    .ds 1
    _set_bkg_submap_PARM_4::    .ds 1
    _set_bkg_submap_PARM_5::    .ds 2
    _set_bkg_submap_PARM_6::    .ds 1
    .xpos:                      .ds 1
    .ypos:                      .ds 1
    .num_rows:                  .ds 1
    .src_tiles:                 .ds 2
    .remainder:                 .ds 1

    .area   _HOME

_set_bkg_submap::
    .define .width      "_set_bkg_submap_PARM_3"
    .define .height     "_set_bkg_submap_PARM_4"
    .define .tiles      "_set_bkg_submap_PARM_5"
    .define .map_width  "_set_bkg_submap_PARM_6"
    sta *.xpos
    stx *.ypos
    lda .tiles
    CLC
    ADC *.xpos
    sta *.src_tiles
    lda .tiles+1
    ADC #0
    sta *.src_tiles+1
    ; += ypos * map_width
    lda *.ypos
    ldx *.map_width
    jsr __muluchar
    clc
    adc *.src_tiles
    sta *.src_tiles
    txa
    adc *.src_tiles+1
    sta *.src_tiles+1
    ;
    lda *.height
    sta *.num_rows
    ; xpos %= DEVICE_SCREEN_WIDTH
    lda *.xpos
    and #.DEVICE_SCREEN_WIDTH-1
    sta *.xpos
    ; ypos %= DEVICE_SCREEN_HEIGHT
    lda #0
    clc
    FAST_MOD8 *.ypos #.DEVICE_SCREEN_HEIGHT
    sta *.ypos
    ; Prefer vertical stripes if height > width
    lda *.height
    cmp *.width
    bcc _set_bkg_submap_horizontalStripes
    beq _set_bkg_submap_horizontalStripes
    jmp _set_bkg_submap_verticalStripes
_set_bkg_submap_horizontalStripes:
    ; Adjust width and calculate remainder if row wraps from right to left
    lda *.xpos
    clc
    adc *.width
    sec
    sbc #.DEVICE_SCREEN_WIDTH
    sta *.remainder
    bmi 1$
    lda *.width
    sec
    sbc *.remainder
    sta *.width
1$:
    ; Decrement to allow treating 0-case same as negative 
    dec *.remainder
_set_bkg_submap_horizontalStripes_rowLoop:
    ; tmp = PPU_NT0 | (ypos << 5) | xpos 
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
    ora #>PPU_NT0
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_horizontal
    ldy #0
    ldx *.width
1$:
    lda [*.src_tiles],y
    iny
    jsr .ppu_stripe_write_byte
    dex
    bne 1$
    jsr .ppu_stripe_end
    ; if wrapped around, write remainder
    lda *.remainder
    bpl _set_bkg_submap_horizontalStripes_remainder
_set_bkg_submap_horizontalStripes_rowLoopEnd:
    ; .src_tiles += .map_width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
    ;inc *.ypos
    ; ypos += 1, with wrap back to 0 if gone past bottom of nametable
    lda *.ypos
    clc
    adc #1
    cmp #.DEVICE_SCREEN_HEIGHT
    bcc 1$
    lda #0
1$:
    sta *.ypos
    dec *.num_rows
    bne _set_bkg_submap_horizontalStripes_rowLoop
    rts

_set_bkg_submap_horizontalStripes_remainder:
    lda *.tmp+1
    ora #>PPU_NT0
    tax
    lda *.tmp
    and #0xE0   ; Always start remainder at X=0
    jsr .ppu_stripe_begin_horizontal
    ldx *.remainder
1$:
    lda [*.src_tiles],y
    iny
    jsr .ppu_stripe_write_byte
    dex
    bpl 1$
    jsr .ppu_stripe_end
    jmp _set_bkg_submap_horizontalStripes_rowLoopEnd


.define .num_cols  ".num_rows"

_set_bkg_submap_verticalStripes:
    ; Adjust height and calculate remainder if row wraps from bottom to top
    lda *.ypos
    clc
    adc *.height
    sec
    sbc #.DEVICE_SCREEN_HEIGHT
    sta *.remainder
    bmi 1$
    lda *.height
    sec
    sbc *.remainder
    sta *.height
1$:
    ; Decrement to allow treating 0-case same as negative 
    dec *.remainder
    ;
    lda *.src_tiles
    sta *.tiles
    lda *.src_tiles+1
    sta *.tiles+1
    ;
    lda *.width
    sta *.num_cols
    ldy #0
_set_bkg_submap_verticalStripes_columnLoop:
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
    ora #>PPU_NT0
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_vertical
    ldx *.height
1$:
    lda [*.src_tiles],y
    jsr .ppu_stripe_write_byte
    ; .src_tiles += width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
    dex
    bne 1$
    jsr .ppu_stripe_end
    ; if wrapped around, write remainder
    lda *.remainder
    bpl _set_bkg_submap_verticalStripes_remainder
_set_bkg_submap_verticalStripes_columnLoopEnd:
    iny
    inc *.xpos
    dec *.num_cols
    bne _set_bkg_submap_verticalStripes_columnLoop
    rts

_set_bkg_submap_verticalStripes_remainder:
    ldx #>PPU_NT0
    lda *.tmp
    and #.DEVICE_SCREEN_WIDTH-1   ; Always start remainder at Y=0
    jsr .ppu_stripe_begin_vertical
    ldx *.remainder
1$:
    lda [*.src_tiles],y
    jsr .ppu_stripe_write_byte
    ; .src_tiles += width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
    dex
    bpl 1$
    jsr .ppu_stripe_end
    jmp _set_bkg_submap_verticalStripes_columnLoopEnd
