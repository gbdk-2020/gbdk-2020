    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_attributes_nes16x16_PARM_3::   .ds 1
    _set_bkg_attributes_nes16x16_PARM_4::   .ds 1
    _set_bkg_attributes_nes16x16_PARM_5::   .ds 2
    .xpos:                                  .ds 1
    .ypos:                                  .ds 1
    .num_columns:                           .ds 1
    .num_rows:                              .ds 1
    .height_second_pass:                    .ds 1
    .height_first_pass:                     .ds 1
    .attribute_x_odd:                       .ds 1
    .starty_second_pass:                    .ds 1
    .x_bits:                                .ds 1
    .attribute_y_odd:                       .ds 1
    .attribute_num_columns_odd:             .ds 1
    .attribute_num_rows_odd:                .ds 1

    .area   _HOME

.define .width  "_set_bkg_attributes_nes16x16_PARM_3"
.define .height "_set_bkg_attributes_nes16x16_PARM_4"
.define .src    "_set_bkg_attributes_nes16x16_PARM_5"

.macro INC_X ?lbl
    ; Increment dst index
    inx
    txa
    bit *.x_bits
    bne lbl
    sec
    sbc #AT_SHADOW_WIDTH
    tax
lbl:
    ; Increment src index
    iny
.endm

_set_bkg_attributes_nes16x16::
    pha
.ifne NT_2H
    ; Two vertical nametables -> wrap around to next
    lda #0x10
.else
    ; One vertical nametable -> wrap around to self
    lda #0x00
.endif
    sta *.starty_second_pass
    txa
    ldy #(.DEVICE_SCREEN_HEIGHT/2)      ; Row end
    cmp #(.DEVICE_SCREEN_HEIGHT/2)
    bcc 1$
    sbc #(.DEVICE_SCREEN_HEIGHT/2)      ; Assumes carry set
.ifne NT_2H
    ldy #0x00
    sty *.starty_second_pass
.endif
    ldy #(2*.DEVICE_SCREEN_HEIGHT/2)    ; Row end
1$:
    clc
    adc *.height
    cmp #17
    bcs .set_bkg_attributes_nes16x16_wrap_two_passes
    ; No wrap - just do single pass
    pla
    jmp _set_bkg_attributes_nes16x16_impl

.set_bkg_attributes_nes16x16_wrap_two_passes:
    tya
    ; num_rows_first = MIN(row_end-y, h);
    sec
    sbc .identity,x
    cmp *.height
    bcc 1$
    lda *.height
1$:
    sta *.height_first_pass
    lda *.height
    sec
    sbc *.height_first_pass
    sta *.height_second_pass
    ; First pass
    lda *.height_first_pass
    beq 11$
    sta *.height
    pla
    pha
    jsr _set_bkg_attributes_nes16x16_impl
11$:
    ; Second pass
    lda *.height_second_pass
    beq 3$
    lsr *.height_first_pass
    sta *.height
    bcs 2$
    ; num_rows_first was even, so attribute data for second pass starts at expected address
    ; Start second pass at AT==16 (next attribute table)
    lda *.starty_second_pass
    tax
    pla
    jmp __set_bkg_attributes_nes16x16_impl_skip_y_adjustment
2$:
    ; num_rows_first was odd, so attribute data for second pass is half-way through expected address
    ; Start second pass at AT y==15 to skip one half-row of attribute data (AT y==15 is always hidden)
    lda *.starty_second_pass
.ifne NT_2H
    ; Two vertical nametables -> wrap around to last row of next nametable
    eor #0x1F
.else
    ; One vertical nametable -> wrap around to last row of this nametable
    eor #0x0F
.endif
    tax
    pla
    jmp __set_bkg_attributes_nes16x16_impl_skip_y_adjustment
3$:
    pla
    rts

_set_bkg_attributes_nes16x16_impl::
    ; Adjust Y-coordinate to skip last unused half-row of 8x8 attribute table
    cpx #(2*AT_HEIGHT-1)
    bcc 42$
    inx
42$:
__set_bkg_attributes_nes16x16_impl_skip_y_adjustment::
    stx *.ypos
    lsr                             ; Make xpos count 32x32 areas / full bytes
    ror *.attribute_x_odd           ; ...and potentially mark x as odd-numbered
    sta *.xpos
    lsr *.ypos                      ; Make ypos count 32x32 areas / full bytes
    ror *.attribute_y_odd           ; ...and potentially mark y as odd-numbered
    lda *.height
    lsr                             ; Make num_rows count 32x32 areas / full bytes
    ror *.attribute_num_rows_odd    ; ...and mark num_rows as odd-numbered
    sta *.num_rows
    lda #AT_SHADOW_WIDTH-1
    sta *.x_bits
    ;
    lda *.width
    lsr                             ; Make num_columns count 32x32 areas / full bytes
    ror *.attribute_num_columns_odd ; ...and mark num_columns as odd-numbered
    sta *.num_columns
    ;
    lda *.ypos
    asl
    asl
    asl
.ifne NT_2W
    asl
.endif
    ora *.xpos
    pha
    tax
    jsr .attribute_set_dirty
    pla
    tax
    ; Branch into distinct routines based on whether x / y are aligned
    ; (even or odd width / height are handled internally by these routines)
    bit *.attribute_y_odd
    bmi 4$
    ; Aligned Y
    bit *.attribute_x_odd
    bmi 2$
    ; Aligned Y, aligned X
    jmp aligned_xy
2$:
    ; Aligned Y, unaligned X
    jmp unaligned_x
4$:
    ; Unaligned Y
    bit *.attribute_x_odd
    bmi 5$
    ; Unaligned Y, aligned X
    jmp unaligned_y
5$:
    ; Unaligned Y, aligned X
    jmp unaligned_xy

;
; Version for when x & y are both aligned to attribute byte-grid
;
.macro ALIGNED_XY_RIGHT_EDGE at_mask at_mask_i ?lbl
    bit *.attribute_num_columns_odd
    bpl lbl
    lda _attribute_shadow,x
    and #at_mask_i
    sta *.tmp
    lda [*.src],y
    and #at_mask
    ora *.tmp
    sta _attribute_shadow,x
lbl:
.endm
;
aligned_xy:
    lda *.num_rows
    beq aligned_xy_row_loop_end
aligned_xy_row_loop:
    jsr .reset_x_coord
aligned_xy_column_loop:
    lda [*.src],y
    sta _attribute_shadow,x
    INC_X
    cpy *.num_columns
    bne aligned_xy_column_loop
    ; If columns were odd, we have one more byte where only left part should be modified
    ALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    jsr .inc_row
    bne aligned_xy_row_loop
aligned_xy_row_loop_end:
    ; If rows were odd, we have one additional row where only top part should be modified
    bit *.attribute_num_rows_odd
    bpl 1$
    jsr .reset_x_coord
2$:
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda [*.src],y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta _attribute_shadow,x
    INC_X
    cpy *.num_columns
    bne 2$
    ; If columns were odd, we have one more byte where only top corner should be modified
    ALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
1$:
    rts

;
; Version for when x is misaligned with attribute byte-grid
;
.define p       ".tmp+1"
.macro UNALIGNED_X_RIGHT_EDGE at_mask at_mask_i ?lbl
    bit *.attribute_num_columns_odd
    bmi lbl
    lda _attribute_shadow,x
    and #at_mask_i
    sta *.tmp
    lda *p
    and #at_mask
    ora *.tmp
    sta _attribute_shadow,x
lbl:
.endm
unaligned_x:
; Round num_columns
    lda *.attribute_num_columns_odd
    bpl 8$
    ; Odd columns get aligned at right edge - but we need to round num_rows upwards!
    inc *.num_columns
8$:
    lda *.num_rows
    beq unaligned_x_row_loop_end
unaligned_x_row_loop:
    jsr .reset_x_coord
    ; As we're writing output +1 X coordinate to the right, first byte in attribute shadow must be a read-modify-write
    ; Fill p with old values for TL / BL, to initialize it for subsequent code.
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL
    sta *p
unaligned_x_column_loop:
    lda [*.src],y
    ; Shift to move right one attribute coordinate, as x is unaligned
    ; Left half of src (top-left, bottom-left) goes into right half (top-right, bottom-right) of current byte...
    asl
    asl
    and #ATTRIBUTE_MASK_TR+ATTRIBUTE_MASK_BR
    ; ...and combined with p providing left half 
    ora *p
    sta _attribute_shadow,x
    ; While right half of src (top-right, bottom right) is saved in p for *next* byte as *left* half
    lda [*.src],y
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL
    sta *p
    INC_X
    cpy *.num_columns
    bne unaligned_x_column_loop
    ; If columns were NOT odd, we have one more byte where only left part should be modified
    UNALIGNED_X_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    jsr .inc_row
    bne unaligned_x_row_loop
unaligned_x_row_loop_end:
    ; If rows were odd, we have one additional row where only top part should be modified
    bit *.attribute_num_rows_odd
    bpl 5$
    jsr .reset_x_coord
    ; As we're writing output +1 X coordinate to the right, first byte in attribute shadow must be a read-modify-write
    ; Fill p with old values for TL / BL / BR, to initialize it for subsequent code.
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *p
2$:
    lda [*.src],y
    ; Shift to move right one attribute coordinate, as x is unaligned
    ; Left half of src (top-left, bottom-left) goes into right half (top-right) of current byte...
    asl
    asl
    and #ATTRIBUTE_MASK_TR
    ; ...and combined with p providing left half 
    ora *p
    sta _attribute_shadow,x
    ; While right half of src (top-right, bottom left, bottom right) is saved in p for *next* byte as *left* half
    lda [*.src],y
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *p
    INC_X
    cpy *.num_columns
    bne 2$
    ; If columns were odd, we have one more byte where only top-left corner should be modified
    UNALIGNED_X_RIGHT_EDGE ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
5$:
    rts

;
; Version for when y is misaligned with attribute byte-grid
;
.macro UNALIGNED_Y_RIGHT_EDGE at_mask at_mask_i ?lbl
    bit *.attribute_num_columns_odd
    bpl lbl
    lda _attribute_shadow,x
    and #at_mask_i
    sta *.tmp
    lda [*.src],y
    asl
    asl
    asl
    asl
    and #ATTRIBUTE_MASK_BL
    ora *.tmp
    ora *pRow,y
    and #at_mask
    sta _attribute_shadow,x
    lda [*.src],y
    lsr
    lsr
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL
    sta *pRow,y
lbl:
.endm
unaligned_y:
.define pRow    "REGTEMP"
; Round num_rows
    lda *.attribute_num_rows_odd
    bpl 9$
    ; Odd rows get aligned at bottom - but we need to round num_rows upwards!
    inc *.num_rows
9$:
    ; Even rows means additional row needed, but don't round num_rows upwards.
; As we're writing output +1 Y coordinate down, first row in attribute shadow must be a read-modify-write
; Fill pRow with old values for TL / TR, to initialize it for subsequent code.
    jsr .reset_x_coord
unaligned_y_row_init_loop:
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y
    INC_X
    cpy *.num_columns
    bne unaligned_y_row_init_loop
    lda *.num_rows
    beq unaligned_y_row_loop_end
unaligned_y_row_loop:
    jsr .reset_x_coord
unaligned_y_column_loop:
    lda [*.src],y
    ; Shift to move down one attribute coordinate, as y is unaligned
    ; Top-part goes into bottom part of current row...
    asl
    asl
    asl
    asl
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    ora *pRow,y
    sta _attribute_shadow,x
    ; ...and bottom part is saved as top part for *next* row
    lda [*.src],y
    lsr
    lsr
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y
    ;
    INC_X
    cpy *.num_columns
    bne unaligned_y_column_loop
    ; If columns were odd, we have one more byte where only left part should be modified
    UNALIGNED_Y_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    jsr .inc_row
    bne unaligned_y_row_loop
unaligned_y_row_loop_end:
    ; If rows were NOT odd, we have one additional row where only top part should be modified
    ; pRow should be used as source data
    bit *.attribute_num_rows_odd
    bmi 1$
    jsr .reset_x_coord
2$:
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda *pRow,y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta _attribute_shadow,x
    INC_X
    cpy *.num_columns
    bne 2$
    ; If columns were odd, we have one more byte where only top-left corner should be modified
    UNALIGNED_Y_RIGHT_EDGE ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
1$:
    rts

;
; Version for when x & y are both misaligned with attribute byte-grid
;
.macro UNALIGNED_XY_RIGHT_EDGE at_mask at_mask_i ?lbl
    bit *.attribute_num_columns_odd
    bmi lbl
    lda _attribute_shadow,x
    and #at_mask_i
    sta *.tmp
    ; top-left taken from previous row...
    lda *pRow,y
    and #ATTRIBUTE_MASK_TL
    ora *.tmp
    ; ...and bottom-left taken from previous *byte*
    sta *.tmp
    lda *p
    asl
    asl
    and #ATTRIBUTE_MASK_BL
    ora *.tmp
    and #at_mask
    sta _attribute_shadow,x
    ; ...finally, save *bottom-left in previous byte moved to top-left*
    lda *p
    lsr
    lsr
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y      ; previous row now correctly setup to contain everything in top-left
lbl:
.endm
unaligned_xy:
; Round num_columns
    lda *.attribute_num_columns_odd
    bpl 8$
    ; Odd columns get aligned at right edge - but we need to round num_rows upwards!
    inc *.num_columns
8$:
    ; Even columns means additional byte needed, but don't round num_columns upwards.
; Round num_rows
    lda *.attribute_num_rows_odd
    bpl 9$
    ; Odd rows get aligned at bottom - but we need to round num_rows upwards!
    inc *.num_rows
9$:
    ; Even rows means additional row needed, but don't round num_rows upwards.
; As we're writing output +1 Y coordinate down, first row in attribute shadow must be a read-modify-write
; Fill pRow with old values for TL / TR, to initialize it for subsequent code. Shift them in X by +1
    jsr .reset_x_coord
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_BL
    sta *p
unaligned_xy_row_init_loop:
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y
    INC_X
    cpy *.num_columns
    bne unaligned_xy_row_init_loop
    lda *.num_rows
    beq unaligned_xy_row_loop_end
unaligned_xy_row_loop:
    jsr .reset_x_coord
unaligned_xy_column_loop:
    lda [*.src],y
    ; Shift to move down one attribute coordinate, and right one attribute coordinate, as x and y are both unaligned
    ; Top-left goes into bottom-right of current row...
    asl
    asl
    asl
    asl
    asl
    asl
    ;and #ATTRIBUTE_MASK_BR
    ; ...with top-left / top-right taken from previous row
    ora *pRow,y
    ; ...and bottom-left taken from previous *byte*
    sta *.tmp
    lda *p
    asl
    asl
    and #ATTRIBUTE_MASK_BL
    ora *.tmp
    sta _attribute_shadow,x
    ; ...finally, bottom-left part is saved for *next* row, but moved to top right and combined with *bottom-left in previous byte moved to top-left*
    lda [*.src],y
    lsr
    lsr
    and #ATTRIBUTE_MASK_TR
    sta *.tmp
    lda *p
    lsr
    lsr
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL
    ora *.tmp
    sta *pRow,y      ; previous row now correctly setup to contain everything in top-left + rop-right
    ; ...and don't forget to save top-right of this byte as new previous byte
    lda [*.src],y
    sta *p
    ;
    INC_X
    cpy *.num_columns
    bne unaligned_xy_column_loop
    ; If columns were NOT odd, we have one more byte where only left part should be modified
    UNALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    jsr .inc_row
    bne unaligned_xy_row_loop
unaligned_xy_row_loop_end:
    ; If rows were NOT odd, we have one additional row where only top part should be modified
    ; pRow should be used as source data - it has already been pre-shifted correctly
    bit *.attribute_num_rows_odd
    bmi 1$
    jsr .reset_x_coord
2$:
    lda _attribute_shadow,x
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda *pRow,y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta _attribute_shadow,x
    INC_X
    cpy *.num_columns
    bne 2$
    ; If columns were NOT odd, we have one more byte where only top-left corner should be modified
    UNALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
1$:
    rts

.attribute_set_dirty:
    ; A = min(AT_HEIGHT-1, .num_rows + .attribute_num_rows_odd) << 3
    lda *.attribute_num_rows_odd
    cmp #0x80
    lda *.num_rows
    adc #0
    cmp #AT_HEIGHT-1
    bcc 1$
    lda #AT_HEIGHT-1
1$:
    sta *.tmp
    ; Y = A | ypos
    lda *.ypos
.ifne NT_2H
    and #AT_HEIGHT-1
    ; Special-case: if we are at last half-row, then ypos actually applies to *next* nametable
    ; So treat it as row == 0 for next nametable
    cmp #AT_HEIGHT-1
    bcc 2$
    bit *.attribute_y_odd
    bpl 2$
    txa
    adc #AT_SHADOW_WIDTH
    and #(AT_SHADOW_WIDTH*AT_SHADOW_HEIGHT-1)
    tax
    lda #0
2$:
.endif
    asl
    asl
    asl
    ora *.tmp
    tay
.ifdef NES_TILEMAP_S
    lda .row_dirty_table,y
    ora *_attribute_row_dirty
    sta *_attribute_row_dirty
.endif
.ifdef NES_TILEMAP_H
    ldx #0
    jsr .mark_left_and_right_at_dirty
.endif
.ifdef NES_TILEMAP_V
    cpx #(AT_SHADOW_WIDTH*AT_SHADOW_HEIGHT/2)
    lda #0
    rol
    tax
    lda .row_dirty_table,y
    ora *_attribute_row_dirty,x
    sta *_attribute_row_dirty,x
.endif
.ifdef NES_TILEMAP_F
    cpx #(AT_SHADOW_WIDTH*AT_SHADOW_HEIGHT/2)
    lda #0
    rol
    asl
    tax
    jsr .mark_left_and_right_at_dirty
.endif
    rts

.ifne NT_2W
;
; Marks left and right attribute table dirty, depending on MSB of xpos and width of attributes to write
;
; Input: X =      0 for NES_TILEMAP_H
;            0 or 2 for NES_TILEMAP_F, indexing dirty flags for top/bottom attribute tables
;
.mark_left_and_right_at_dirty:
    .define .flip_xpos_msb      "DPTR"
    .define .x_wrapped_around   "DPTR+1"
    lda #AT_WIDTH
    sta *.flip_xpos_msb
    ; Store wrapped-around flag to same bit as xpos MSB (AT_WIDTH)
    lda *.xpos
    sec     ; +1 to account for rounding odd coordinates upwards
    adc *.num_columns
    eor *.xpos
    and #AT_WIDTH
    sta *.x_wrapped_around
    ; First loop iteration:  Mark left AT dirty if xpos < AT_WIDTH or wrap-around occurred
    ; Second loop iteration: Mark right AT dirty if xpos >= AT_WIDTH or wrap-around occurred
9$:
    lda *.xpos
    eor *.flip_xpos_msb
    ora *.x_wrapped_around
    and #AT_WIDTH
    beq 10$
    lda .row_dirty_table,y
    ora *_attribute_row_dirty,x
    sta *_attribute_row_dirty,x
10$:
    inx
    lda *.flip_xpos_msb
    eor #AT_WIDTH
    sta *.flip_xpos_msb
    beq 9$
    rts
.endif

.reset_x_coord:
    ldy #0
    txa
    and #((AT_SHADOW_HEIGHT - 1) * AT_SHADOW_WIDTH)
    ora *.xpos
    tax
    rts

.inc_row:
    ; src += num_columns
    lda *.src
    clc
    adc *.num_columns
    sta *.src
    lda *.src+1
    adc #0
    sta *.src+1
    ; Increment Y-coordinate of attribute shadow index
    txa
    adc #AT_SHADOW_WIDTH
    and #(AT_SHADOW_WIDTH*AT_SHADOW_HEIGHT-1)
    tax
    ; Decrement num_rows for caller loop
    dec *.num_rows
    rts

;
; Provides a quick table lookup to set _attribute_row_dirty based on num_rows / ypos
;
; Indexing: 00YYYNNN
;
; Where:
;   Y = Starting byte row in attribute table
;   N = Number of byte rows in attribute table -1 (including additional row if needed)
;
.row_dirty_table:
; Y = 0
.db 0b00000001
.db 0b00000011
.db 0b00000111
.db 0b00001111
.db 0b00011111
.db 0b00111111
.db 0b01111111
.db 0b11111111
; Y = 1
.db 0b00000010
.db 0b00000110
.db 0b00001110
.db 0b00011110
.db 0b00111110
.db 0b01111110
.db 0b11111110
.db 0b11111111
; Y = 2
.db 0b00000100
.db 0b00001100
.db 0b00011100
.db 0b00111100
.db 0b01111100
.db 0b11111100
.db 0b11111101
.db 0b11111111
; Y = 3
.db 0b00001000
.db 0b00011000
.db 0b00111000
.db 0b01111000
.db 0b11111000
.db 0b11111001
.db 0b11111011
.db 0b11111111
; Y = 4
.db 0b00010000
.db 0b00110000
.db 0b01110000
.db 0b11110000
.db 0b11110001
.db 0b11110011
.db 0b11110111
.db 0b11111111
; Y = 5
.db 0b00100000
.db 0b01100000
.db 0b11100000
.db 0b11100001
.db 0b11100011
.db 0b11100111
.db 0b11101111
.db 0b11111111
; Y = 6
.db 0b01000000
.db 0b11000000
.db 0b11000001
.db 0b11000011
.db 0b11000111
.db 0b11001111
.db 0b11011111
.db 0b11111111
; Y = 7
.db 0b10000000
.db 0b10000001
.db 0b10000011
.db 0b10000111
.db 0b10001111
.db 0b10011111
.db 0b10111111
.db 0b11111111
