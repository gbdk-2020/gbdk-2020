    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_submap_attributes_nes16x16_PARM_3::    .ds 1
    _set_bkg_submap_attributes_nes16x16_PARM_4::    .ds 1
    _set_bkg_submap_attributes_nes16x16_PARM_5::    .ds 2
    _set_bkg_submap_attributes_nes16x16_PARM_6::    .ds 1
    .xpos:                                          .ds 1
    .ypos:                                          .ds 1
    .num_rows:                                      .ds 1
    .src_tiles:                                     .ds 2
    .x_odd:                                         .ds 1
    .y_odd:                                         .ds 1
    .num_columns:                                   .ds 1
    .attribute_mask_map:                            .ds 1
    .attribute_mask_shadow:                         .ds 1
    .row_shl_3:                                     .ds 1

    .area   _HOME

.macro INC_X_WITH_WRAP
    txa
    clc
    adc #1
    and #ATTRIBUTE_PACKED_WIDTH-1
    ora *.row_shl_3
    tax
.endm

.macro INC_XPOS_WITH_WRAP
    ldx *.xpos
    clc
    adc #1
    and #ATTRIBUTE_PACKED_WIDTH-1
    stx *.xpos
.endm

.macro INC_ROW_SRC
    ; .src_tiles += width
    lda *.map_width
    clc
    adc *.src_tiles
    sta *.src_tiles
    lda #0
    adc *.src_tiles+1
    sta *.src_tiles+1
.endm

.macro INC_Y_WITH_WRAP
    txa
    clc
    adc #(1 << 3)
    and #0x3F
    tax
    INC_ROW_SRC
.endm

_set_bkg_submap_attributes_nes16x16::
    .define .width      "_set_bkg_submap_attributes_nes16x16_PARM_3"
    .define .height     "_set_bkg_submap_attributes_nes16x16_PARM_4"
    .define .tiles      "_set_bkg_submap_attributes_nes16x16_PARM_5"
    .define .map_width  "_set_bkg_submap_attributes_nes16x16_PARM_6"
    lsr
    sta *.xpos
    ror *.x_odd
    ; Attribute map stores 8 rows of bytes with 2x2 attributes in each byte continuously.
    ; -> 16 rows of attributes
    ; But for alignment reasons, the last row only stores 2x1 (bottom-left / bottom-right unused)
    ; This extra row must be skipped to remap input y position into a new attribute table Y position 
    ; Rather than using a division to compensate, we use the following rules:
    ;   Y = y / 16
    ;   For each AT n already covered vertically:
    ;   - increase Y by +1
    ;   - also increase Y by +1 to skip 1 row of 16x16 attributes (occurs 1 coordinate earlier each AT)
    ; Or in code:
    ;   n = y >> 4
    ;   Y = y + n + (y & 0xF >= 15 - n) ? 1 : 0
    txa
    lsr
    lsr
    lsr
    lsr
    sta *.tmp
    eor #0xF
    sta *.tmp+1
    txa
    and #0xF
    cmp *.tmp+1
    txa
    adc *.tmp
    ;
    lsr
    sta *.ypos
    ror *.y_odd
    lda *.tiles
    clc
    adc *.xpos
    sta *.src_tiles
    lda *.tiles+1
    adc #0
    sta *.src_tiles+1
    lda *.map_width
    lsr
    sta *.map_width
    ldx *.ypos
    jsr __muluchar
    clc
    adc *.src_tiles
    sta *.src_tiles
    txa
    adc *.src_tiles+1
    sta *.src_tiles+1
    ; xpos %= ATTRIBUTE_PACKED_WIDTH
    lda *.xpos
    and #ATTRIBUTE_PACKED_WIDTH-1
    sta *.xpos
    ; ypos %= ATTRIBUTE_PACKED_HEIGHT
    lda *.ypos
    and #ATTRIBUTE_PACKED_HEIGHT-1
    sta *.ypos
    ; Prefer vertical stripes if height > width
    lda *.height
    cmp *.width
    bcc _set_bkg_submap_attributes_horizontalStripes
    beq _set_bkg_submap_attributes_horizontalStripes
    jmp _set_bkg_submap_attributes_verticalStripes
_set_bkg_submap_attributes_horizontalStripes:
    lda *.height
    sta *.num_rows
_set_bkg_submap_attributes_horizontalStripes_rowLoop:
    ;
    ldy *.ypos
    lda .bitmask_table,y
    ora *_attribute_row_dirty
    sta *_attribute_row_dirty
    ;
    jsr .process_row
    jsr .inc_row
    dec *.num_rows
    bne _set_bkg_submap_attributes_horizontalStripes_rowLoop  
    rts

_set_bkg_submap_attributes_verticalStripes:
    jsr .inc_height_if_wrap
    lda *.width
    sta *.num_columns
    ldy #0
_set_bkg_submap_attributes_verticalStripes_columnLoop:
    ;
    ldx *.xpos
    lda .bitmask_table,x
    ora *_attribute_column_dirty
    sta *_attribute_column_dirty
    ;
    jsr .process_column
    INC_XPOS_WITH_WRAP
    iny
    dec *.num_columns
    bne _set_bkg_submap_attributes_verticalStripes_columnLoop  
    rts

.inc_row:
    lda *.ypos
    cmp #ATTRIBUTE_PACKED_HEIGHT-1
    beq 2$
    lda *.y_odd
    adc #0x80
    sta *.y_odd
    bmi 1$
    lda *.ypos
    clc
    adc #1
    and #ATTRIBUTE_PACKED_HEIGHT-1
    sta *.ypos
    bpl .inc_row_src
1$:
    rts
2$:
    ; Skip last 16x16 row of attribute table (empty due to alignment)
    lda #0
    sta *.ypos
.inc_row_src:
    INC_ROW_SRC
    rts

.process_row:
    ; Write only top part of row
    lda #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    bit *.y_odd
    bpl 1$
    ; Shift mask to write only bottom part of row
    asl
    asl
    asl
    asl
1$:
    sta *.attribute_mask_map
    ; Invert mask
    eor #0xFF
    sta *.attribute_mask_shadow
;
; width even, x even        -> No pre-loop, no post-loop
; -> num_columns = w/2, pre = 0, post = 0
; width even, x odd         -> Pre-loop writing right half, post-loop writing left half
; -> num_columns = w/2-1, pre = 1, post = 1
; width odd, x even         -> No Pre-loop, post-loop writing left half
; -> num_columns = w/2, pre = 0, post = 1
; width odd, x odd          ->  Pre-loop writing right half,no post-loop
; -> num_columns = w/2, pre = 1, post = 0
; width 1, x even           -> Pre-loop only. (special case)
; -> num_columns = w/2, pre = 1, post = 0 
;
; +-----+-----+-----+
; | X\W |  0  |  1  |
; +-----+-----+-----+
; |  0  | 0/0 | 0/1 |
; +-----+-----+-----+
; |  1  | 1/1 | 1/0 |
; +-----+-----+-----+
;
; pre? = x_odd
; post? = x_odd XOR width_odd
    lda *.width
    lsr
    sta *.num_columns
    lda *.ypos
    asl
    asl
    asl
    sta *.row_shl_3
    lda *.xpos
    ora *.row_shl_3
    tax
    ldy #0
    bit *.x_odd
    bpl 2$
    ; Do a partial update of only TR+BR for the first byte
    lda *.attribute_mask_map
    and #ATTRIBUTE_MASK_TR+ATTRIBUTE_MASK_BR
    pha
    and [*.src_tiles],y
    iny
    sta *.tmp
    pla
    eor #0xFF
    and _attribute_shadow,x
    ora *.tmp
    sta _attribute_shadow,x
    INC_X_WITH_WRAP
;;;
    lda *.width
    lsr
    bcs 2$
;;;
    dec *.num_columns
2$:
    lda *.num_columns
    beq .column_loop_done

.column_loop:
    lda [*.src_tiles],y
    iny
    and *.attribute_mask_map
    sta *.tmp
    lda _attribute_shadow,x
    and *.attribute_mask_shadow
    ora *.tmp
    sta _attribute_shadow,x
    INC_X_WITH_WRAP
    dec *.num_columns
    bne .column_loop
    ;
.column_loop_done:
    lda *.width
    lsr
    ror
    eor *.x_odd 
    bpl 1$
    ; We have one remaining half-column (16 pixels wide)
    ; Do a partial update of only TL+BL for the last column
    lda *.attribute_mask_map
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL
    pha
    and [*.src_tiles],y
    iny
    sta *.tmp
    pla
    eor #0xFF
    and _attribute_shadow,x
    ora *.tmp
    sta _attribute_shadow,x
1$:
    rts

.inc_height_if_wrap:
    ; Aligned AT handling: if ypos will wrap from 14 -> 0, then height should be += 1
    lda *.y_odd
    cmp #0x80
    lda *.ypos
    rol
    tax
    clc
    adc *.height
    cmp #ATTRIBUTE_HEIGHT
    bcc 3$
    inc *.height
3$:
    txa
    lsr
    sta *.ypos
    ror *.y_odd
    rts

.process_column:
    ; Write only left part of column
    lda #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL
    bit *.x_odd
    bpl 1$
    ; Shift mask to write only right part of column
    asl
    asl
1$:
    sta *.attribute_mask_map
    ; Invert mask
    eor #0xFF
    sta *.attribute_mask_shadow
    lda *.height
    lsr
    sta *.num_rows
    lda *.ypos
    asl
    asl
    asl
    sta *.row_shl_3
    lda *.xpos
    ora *.row_shl_3
    tax
    bit *.y_odd
    bpl 2$
    ; Do a partial update of only BL+BR for the first byte
    lda *.attribute_mask_map
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    pha
    and [*.src_tiles],y
    sta *.tmp
    pla
    eor #0xFF
    and _attribute_shadow,x
    ora *.tmp
    sta _attribute_shadow,x
    INC_Y_WITH_WRAP
;;;
    lda *.height
    lsr
    bcs 2$
;;;
    dec *.num_rows
2$:
    lda *.num_rows
    beq .row_loop_done

.row_loop:
    lda [*.src_tiles],y
    and *.attribute_mask_map
    sta *.tmp
    lda _attribute_shadow,x
    and *.attribute_mask_shadow
    ora *.tmp
    sta _attribute_shadow,x
    INC_Y_WITH_WRAP
    dec *.num_rows
    bne .row_loop
    ;
.row_loop_done:
    lda *.height
    lsr
    ror
    eor *.y_odd 
    bpl 1$
    ; We have one remaining half-row (16 pixels tall)
    ; Do a partial update of only TL+TR for the last byte
    lda *.attribute_mask_map
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    pha
    and [*.src_tiles],y
    iny
    sta *.tmp
    pla
    eor #0xFF
    and _attribute_shadow,x
    ora *.tmp
    sta _attribute_shadow,x
1$:
    rts

.bitmask_table:
.db 0b00000001
.db 0b00000010
.db 0b00000100
.db 0b00001000
.db 0b00010000
.db 0b00100000
.db 0b01000000
.db 0b10000000
