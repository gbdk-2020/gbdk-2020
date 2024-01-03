    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_attributes_nes16x16_PARM_3::   .ds 1
    _set_bkg_attributes_nes16x16_PARM_4::   .ds 1
    _set_bkg_attributes_nes16x16_PARM_5::   .ds 2
    .xpos:                                  .ds 1
    .ypos:                                  .ds 1
    .num_columns:                           .ds 1
    .num_rows:                              .ds 1
    .src:                                   .ds 2
    .dst:                                   .ds 2
    .attribute_x_odd:                       .ds 1
    .attribute_y_odd:                       .ds 1
    .attribute_num_columns_odd:             .ds 1
    .attribute_num_rows_odd:                .ds 1

    .area   _HOME

.define .width  "_set_bkg_attributes_nes16x16_PARM_3"
.define .height "_set_bkg_attributes_nes16x16_PARM_4"
.define .tiles  "_set_bkg_attributes_nes16x16_PARM_5"

;
; Fast version writing directly to PPU memory.
; Does not handle unaligned x & y and assumes even number of columns / rows
;
_set_bkg_attributes_nes16x16_fast::
1$:
    lda *.ypos
    asl
    asl
    asl
    ora *.xpos
    sta *.tmp
    lda #>PPU_AT0
    sta PPUADDR
    lda *.tmp
    ora #<PPU_AT0
    sta PPUADDR
    ldx *.tmp
    ldy #0
2$:
    lda [*.src],y
    sta PPUDATA
    sta _attribute_shadow,x
    inx
    iny
    cpy *.num_columns
    bne 2$
    ; .src += y
    tya
    clc
    adc *.src
    sta *.src
    lda #0
    adc *.src+1
    sta *.src+1
    inc *.ypos
    dec *.num_rows
    bne 1$
    rts

_set_bkg_attributes_nes16x16::
    lsr                             ; Make xpos count 32x32 areas / full bytes
    ror *.attribute_x_odd           ; ...and potentially mark x as odd-numbered
    sta *.xpos
    stx *.ypos
    lsr *.ypos                      ; Make ypos count 32x32 areas / full bytes
    ror *.attribute_y_odd           ; ...and potentially mark y as odd-numbered
    lda *.tiles
    sta *.src
    lda *.tiles+1
    sta *.src+1
    lda *.height
    lsr                             ; Make num_rows count 32x32 areas / full bytes
    ror *.attribute_num_rows_odd    ; ...and mark num_rows as odd-numbered
    sta *.num_rows
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
    ora *.xpos
    clc
    adc #<_attribute_shadow
    sta *.dst
    lda #>_attribute_shadow
    adc #0
    sta *.dst+1
    jsr .attribute_set_dirty
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

; Boilerplate code for row-loop
.macro INC_SRC_AND_DST
    ; src += num_columns
    lda *.src
    clc
    adc *.num_columns
    sta *.src
    lda *.src+1
    adc #0
    sta *.src+1
    ; dst += 8 (will never wrap boundary)
    lda *.dst
    adc #8
    sta *.dst
.endm


;
; Version for when x & y are both aligned to attribute byte-grid
;
.macro ALIGNED_XY_RIGHT_EDGE at_mask at_mask_i ?lbl
    bit *.attribute_num_columns_odd
    bpl lbl
    lda [*.dst],y
    and #at_mask_i
    sta *.tmp
    lda [*.src],y
    and #at_mask
    ora *.tmp
    sta [*.dst],y
lbl:
.endm
;
aligned_xy:
aligned_xy_row_loop:
    ldy #0
aligned_xy_column_loop:
    lda [*.src],y
    sta [*.dst],y
    iny
    cpy *.num_columns
    bne aligned_xy_column_loop
    ; If columns were odd, we have one more byte where only left part should be modified
    ALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    INC_SRC_AND_DST
    dec *.num_rows
    bne aligned_xy_row_loop
    ; If rows were odd, we have one additional row where only top part should be modified
    bit *.attribute_num_rows_odd
    bpl 1$
    ldy #0
2$:
    lda [*.dst],y
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda [*.src],y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta [*.dst],y
    iny
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
    lda [*.dst],y
    and #at_mask_i
    sta *.tmp
    lda *p
    and #at_mask
    ora *.tmp
    sta [*.dst],y
lbl:
.endm
unaligned_x:
; Round num_columns
    lda *.attribute_num_columns_odd
    bpl 8$
    ; Odd columns get aligned at right edge - but we need to round num_rows upwards!
    inc *.num_columns
8$:
unaligned_x_row_loop:
    ldy #0
    ; As we're writing output +1 X coordinate to the right, first byte in attribute shadow must be a read-modify-write
    ; Fill p with old values for TL / BL, to initialize it for subsequent code.
    lda [*.dst],y
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
    sta [*.dst],y
    ; While right half of src (top-right, bottom right) is saved in p for *next* byte as *left* half
    lda [*.src],y
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL
    sta *p
    iny
    cpy *.num_columns
    bne unaligned_x_column_loop
    ; If columns were NOT odd, we have one more byte where only left part should be modified
    UNALIGNED_X_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    INC_SRC_AND_DST
    dec *.num_rows
    bne unaligned_x_row_loop
    ; If rows were odd, we have one additional row where only top part should be modified
    bit *.attribute_num_rows_odd
    bpl 5$
    ldy #0
    ; As we're writing output +1 X coordinate to the right, first byte in attribute shadow must be a read-modify-write
    ; Fill p with old values for TL / BL / BR, to initialize it for subsequent code.
    lda [*.dst],y
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
    sta [*.dst],y
    ; While right half of src (top-right, bottom left, bottom right) is saved in p for *next* byte as *left* half
    lda [*.src],y
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *p
    iny
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
    lda [*.dst],y
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
    sta [*.dst],y
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
    ldy *.num_columns
    dey
unaligned_y_row_init_loop:
    lda [*.dst],y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y
    dey
    bpl unaligned_y_row_init_loop
unaligned_y_row_loop:
    ldy #0
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
    sta [*.dst],y
    ; ...and bottom part is saved as top part for *next* row
    lda [*.src],y
    lsr
    lsr
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    sta *pRow,y
    ;
    iny
    cpy *.num_columns
    bne unaligned_y_column_loop
    ; If columns were odd, we have one more byte where only left part should be modified
    UNALIGNED_Y_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    INC_SRC_AND_DST
    dec *.num_rows
    bne unaligned_y_row_loop
    ; If rows were NOT odd, we have one additional row where only top part should be modified
    ; pRow should be used as source data
    bit *.attribute_num_rows_odd
    bmi 1$
    ldy #0
2$:
    lda [*.dst],y
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda *pRow,y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta [*.dst],y
    iny
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
    lda [*.dst],y
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
    sta [*.dst],y
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
    ldy #0
    lda [*.dst],y
    and #ATTRIBUTE_MASK_TL
    sta *p
unaligned_xy_row_init_loop:
    lda [*.dst],y
    ; TL -> TR
    asl
    asl
    and #ATTRIBUTE_MASK_TR
    ; Combine with TR -> TL from previous
    ora *p
    sta *pRow,y
    lda [*.dst],y
    ; TR -> TL for next
    lsr
    lsr
    and #ATTRIBUTE_MASK_TL
    sta *p
    iny
    cpy *.num_columns
    bne unaligned_xy_row_init_loop
unaligned_xy_row_loop:
    ldy #0
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
    sta [*.dst],y
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
    iny
    cpy *.num_columns
    bne unaligned_xy_column_loop
    ; If columns were NOT odd, we have one more byte where only left part should be modified
    UNALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
    INC_SRC_AND_DST
    dec *.num_rows
    bne unaligned_xy_row_loop
    ; If rows were NOT odd, we have one additional row where only top part should be modified
    ; pRow should be used as source data - it has already been pre-shifted correctly
    bit *.attribute_num_rows_odd
    bmi 1$
    ldy #0
2$:
    lda [*.dst],y
    and #ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR
    sta *.tmp
    lda *pRow,y
    and #ATTRIBUTE_MASK_TL+ATTRIBUTE_MASK_TR
    ora *.tmp
    sta [*.dst],y
    iny
    cpy *.num_columns
    bne 2$
    ; If columns were NOT odd, we have one more byte where only top-left corner should be modified
    UNALIGNED_XY_RIGHT_EDGE ATTRIBUTE_MASK_TL, ATTRIBUTE_MASK_BL+ATTRIBUTE_MASK_BR+ATTRIBUTE_MASK_TR
1$:
    rts

.attribute_set_dirty:
    ; A = min(7, .num_rows + .attribute_num_rows_odd) << 3
    lda *.attribute_num_rows_odd
    cmp #0x80
    lda *.num_rows
    adc #0
    cmp #7
    bcc 1$
    lda #7
1$:
    sta *.tmp
    ; X = A | ypos
    lda *.ypos
    asl
    asl
    asl
    ora *.tmp
    tax
    lda .row_dirty_table,x
    sta *_attribute_row_dirty
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
.db 0b11111110
; Y = 2
.db 0b00000100
.db 0b00001100
.db 0b00011100
.db 0b00111100
.db 0b01111100
.db 0b11111100
.db 0b11111100
.db 0b11111100
; Y = 3
.db 0b00001000
.db 0b00011000
.db 0b00111000
.db 0b01111000
.db 0b11111000
.db 0b11111000
.db 0b11111000
.db 0b11111000
; Y = 4
.db 0b00010000
.db 0b00110000
.db 0b01110000
.db 0b11110000
.db 0b11110000
.db 0b11110000
.db 0b11110000
.db 0b11110000
; Y = 5
.db 0b00100000
.db 0b01100000
.db 0b11100000
.db 0b11100000
.db 0b11100000
.db 0b11100000
.db 0b11100000
.db 0b11100000
; Y = 6
.db 0b01000000
.db 0b11000000
.db 0b11000000
.db 0b11000000
.db 0b11000000
.db 0b11000000
.db 0b11000000
.db 0b11000000
; Y = 7
.db 0b10000000
.db 0b10000000
.db 0b10000000
.db 0b10000000
.db 0b10000000
.db 0b10000000
.db 0b10000000
.db 0b10000000
