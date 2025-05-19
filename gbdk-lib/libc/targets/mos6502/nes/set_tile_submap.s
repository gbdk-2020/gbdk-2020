    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_submap_PARM_3::
    _set_bkg_based_submap_PARM_3::  .ds 1
    _set_bkg_submap_PARM_4::
    _set_bkg_based_submap_PARM_4::  .ds 1
    _set_bkg_submap_PARM_5::
    _set_bkg_based_submap_PARM_5::  .ds 2
    _set_bkg_submap_PARM_6::
    _set_bkg_based_submap_PARM_6::  .ds 1
    _set_bkg_based_submap_PARM_7::  .ds 1
    .xpos:                          .ds 1
    .ypos:                          .ds 1
    .num_rows:                      .ds 1
    .src_tiles:                     .ds 2
    .remainder:                     .ds 1
    .ppuhi:                         .ds 1
    .stripe_loop_counter:           .ds 1

    .define .width          "_set_bkg_submap_PARM_3"
    .define .height         "_set_bkg_submap_PARM_4"
    .define .tiles          "_set_bkg_submap_PARM_5"
    .define .map_width      "_set_bkg_submap_PARM_6"
    .define .tile_offset    "_set_bkg_based_submap_PARM_7"

    .area   _HOME

.ifdef NES_TILEMAP_S
.define PPUHI_MASK "#>PPU_NT0"
.else
.define PPUHI_MASK "*.ppuhi"
.endif

_set_bkg_submap::
    ldy #0
    sty *.tile_offset
_set_bkg_based_submap::
    sta *.xpos
    stx *.ypos
    lda .tiles
    clc
    adc *.xpos
    sta *.src_tiles
    lda .tiles+1
    adc #0
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
.set_bkg_common::
    ;
    lda *.height
    sta *.num_rows
.ifne NT_2W
    ; .ppuhi = (xpos >> 3) & 0b00000100
    lda *.xpos
    lsr
    lsr
    lsr
    and #0b00000100
    ora #>PPU_NT0
    sta *.ppuhi
.else
.ifne NT_2H
    lda #>PPU_NT0
    sta *.ppuhi
.endif
.endif
    ; xpos %= NT_WIDTH
    lda *.xpos
    and #NT_WIDTH-1
    sta *.xpos
    ldx *.ypos
    ; ypos %= NT_HEIGHT
    lda *.ypos
    jsr .div_mod_height
    sta *.ypos
.ifne NT_2H
    ; .ppuhi |= (ypos % DEVICE_SCREEN_BUFFER_HEIGHT) & 0b00001000
    txa
    asl
    asl
    asl
    and #0b00001000
    ora *.ppuhi
    sta *.ppuhi
.endif
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
    sbc #NT_WIDTH
    sta *.remainder
    bmi 1$
    lda *.width
    sec
    sbc *.remainder
    sta *.width
1$:
_set_bkg_submap_horizontalStripes_rowLoop:
    ldx *.width
    stx *.stripe_loop_counter
    jsr .setup_stripe_address
    ldy #0
    ldx *.tmp+1
    lda *.tmp
    jsr .write_horizontal_stripe
    ; if wrapped around, write remainder
    lda *.remainder
    bmi _set_bkg_submap_horizontalStripes_rowLoopEnd
    bne _set_bkg_submap_horizontalStripes_remainder
_set_bkg_submap_horizontalStripes_rowLoopEnd:
    ; .src_tiles += .map_width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    bcc 2$
    inc *.src_tiles+1
2$:
    ; ypos += 1, with wrap back to 0 if gone past bottom of nametable
    lda *.ypos
    clc
    adc #1
    cmp #NT_HEIGHT
    bcc 1$
    adc #0xE1 ; 0x02-0x20-C   ; (carry assumed set)
.ifne NT_2H
    ; Flip nametable Y bit after storing wrapped ypos
    sta *.ypos
    lda *.ppuhi
    eor #0b00001000
    sta *.ppuhi
    bcs 3$   ; Carry still set, use BCS in place of JMP
.endif
1$:
    sta *.ypos
3$:
    dec *.num_rows
    bne _set_bkg_submap_horizontalStripes_rowLoop
    rts

_set_bkg_submap_horizontalStripes_remainder:
    ldx *.remainder
    stx *.stripe_loop_counter
.ifne NT_2W + NT_2H
    lda *.tmp+1
    ora *.ppuhi
    eor #0b00000100
    tax
.else
    ldx *.tmp+1
.endif
    lda *.tmp
    and #0xE0   ; Always start remainder at X=0
    jsr .write_horizontal_stripe
    jmp _set_bkg_submap_horizontalStripes_rowLoopEnd


.define .num_cols  ".num_rows"

_set_bkg_submap_verticalStripes:
    ; Adjust height and calculate remainder if row wraps from bottom to top
    lda *.ypos
    clc
    adc *.height
    sec
    sbc #NT_HEIGHT
    sta *.remainder
    bmi 1$
    lda *.height
    sec
    sbc *.remainder
    sta *.height
1$:
    lda *.src_tiles
    sta *.tiles
    lda *.src_tiles+1
    sta *.tiles+1
    ;
    lda *.width
    sta *.num_cols
    ldy #0
_set_bkg_submap_verticalStripes_columnLoop:
    ldx *.height
    stx *.stripe_loop_counter
    lda *.tiles
    sta *.src_tiles
    lda *.tiles+1
    sta *.src_tiles+1
    ;
    jsr .setup_stripe_address
    ldx *.tmp+1
    lda *.tmp
    jsr .write_vertical_stripe
    ; if wrapped around, write remainder
    lda *.remainder
    bmi _set_bkg_submap_verticalStripes_columnLoopEnd
    bne _set_bkg_submap_verticalStripes_remainder
_set_bkg_submap_verticalStripes_columnLoopEnd:
    iny
    inc *.xpos
    dec *.num_cols
    bne _set_bkg_submap_verticalStripes_columnLoop
    rts

_set_bkg_submap_verticalStripes_remainder:
    ldx *.remainder
    stx *.stripe_loop_counter
.ifne NT_2H
    lda *.ppuhi
    eor #0b00001000
    tax
.else
    ldx PPUHI_MASK
.endif
    lda *.tmp
    and #NT_WIDTH-1   ; Always start remainder at Y=0
    jsr .write_vertical_stripe
    jmp _set_bkg_submap_verticalStripes_columnLoopEnd

.div_mod_height:
    ldx #0
    sec
1$:
    sbc #NT_HEIGHT
    bcc 2$
    inx
    jmp 1$
2$:
    adc #NT_HEIGHT
    rts

.setup_stripe_address:
    ; tmp = ppuhi | (ypos << 5) | xpos
    lda #(PPU_NT0 >> 13)
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
.ifne NT_2W + NT_2H
    ora PPUHI_MASK
.endif
    sta *.tmp+1
    rts

.write_horizontal_stripe:
    jsr .ppu_stripe_begin_horizontal
1$:
    lda [*.src_tiles],y
    iny
    clc
    adc *.tile_offset
    jsr .ppu_stripe_write_byte
    dec *.stripe_loop_counter
    bne 1$
    jmp .ppu_stripe_end

.write_vertical_stripe:
    jsr .ppu_stripe_begin_vertical
1$:
    lda [*.src_tiles],y
    clc
    adc *.tile_offset
    jsr .ppu_stripe_write_byte
    ; .src_tiles += width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    bcc 2$
    inc *.src_tiles+1
2$:
    dec *.stripe_loop_counter
    bne 1$
    jmp .ppu_stripe_end
