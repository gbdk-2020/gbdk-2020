    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    .x_save:                    .ds 1
    .y_save:                    .ds 1
    .attribute_row_dirty:       .ds 1
    .attribute_column_dirty:    .ds 1

    .area   _HOME

.macro LOAD_ROW_DIRTY ?lbl, ?lbl2
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl lbl
    lda *_attribute_row_dirty_win,x
    jmp lbl2
.endif
lbl:
    lda *_attribute_row_dirty,x
lbl2:
.endm

.macro LOAD_COLUMN_DIRTY ?lbl, ?lbl2
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl lbl
    lda *_attribute_column_dirty_win,x
    jmp lbl2
.endif
lbl:
    lda *_attribute_column_dirty,x
lbl2:
.endm

.macro LOAD_ROW_ATTRIBUTE ?lbl, ?lbl2
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl lbl
    lda *_attribute_row_dirty_win,x
    jmp lbl2
.endif
lbl:
    lda *_attribute_row_dirty,x
lbl2:
.endm

.macro CLEAR_ATTRIBUTE_DIRTY ?lbl, ?lbl2
    lda #0
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl lbl
    sta _attribute_row_dirty_win,x
    sta _attribute_column_dirty_win,x
    jmp lbl2
.endif
lbl:
    sta _attribute_row_dirty,x
    sta _attribute_column_dirty,x
lbl2:
.endm

_flush_shadow_attributes::
.ifdef NES_WINDOW_LAYER
    ; First process window layer
    lda *__current_vram_cfg_write
    pha
    ora #PPUHI_WIN
    sta *__current_vram_cfg_write
    jsr _flush_shadow_attributes_impl
    ; ...then background layer
    lda *__current_vram_cfg_write
    and #~PPUHI_WIN
    sta *__current_vram_cfg_write
    jsr _flush_shadow_attributes_impl
    pla
    sta *__current_vram_cfg_write
    rts
_flush_shadow_attributes_impl::
.endif
    ldx #0
.ifndef NES_TILEMAP_S
1$:
.endif
    stx *.x_save
    LOAD_ROW_DIRTY
    beq 2$
    sta *.attribute_row_dirty
    ldy .xy_shift_tab,x
    jsr _flush_shadow_attributes_rows
    ldx *.x_save
2$:
    LOAD_COLUMN_DIRTY
    beq 3$
    sta *.attribute_column_dirty
    ldy .xy_shift_tab,x
    jsr _flush_shadow_attributes_columns
3$:
    ldx *.x_save
    CLEAR_ATTRIBUTE_DIRTY
.ifndef NES_TILEMAP_S
    inx
    cpx #NUM_NT
    bne 1$
.endif
    rts

;
; Writes every row of attributes from _shadow_attributes that's been marked
; as dirty in the _attribute_row_dirty byte to PPU memory.
;
_flush_shadow_attributes_rows:
    lda #<PPU_AT0
    sta *.tmp
    lda .ppu_hi_tab,x
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl 0$
    ora #PPUHI_WIN
0$:
.endif
    sta *.tmp+1
_flush_shadow_attributes_row_loop:
    lsr *.attribute_row_dirty
    bcc 1$
    jmp _flush_shadow_attributes_update_row
1$:
    beq _flush_shadow_attributes_end
_flush_shadow_attributes_next_row:
    ; Y += AT_SHADOW_WIDTH
    tya
    clc
    adc #AT_SHADOW_WIDTH
    tay
    ; .tmp += ATTRIBUTE_PACKED_WIDTH
    lda *.tmp
    adc #ATTRIBUTE_PACKED_WIDTH
    sta *.tmp
    jmp _flush_shadow_attributes_row_loop
_flush_shadow_attributes_end:
    rts

;
; Flushes all dirty rows of _attribute_shadow by writing them to PPU memory
;
_flush_shadow_attributes_update_row:
    stx *REGTEMP+3
    ; Update all 8 bytes of row for now, as each row in _attribute_row_dirty only stores 1 bit
    ; TODO: Could store 8 bytes and update range, at expense of 7 more bytes.
    lda *.tmp+1
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_horizontal
    ; Write 8 bytes
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl 1$
    i = 0
    .rept ATTRIBUTE_PACKED_WIDTH
    lda _attribute_shadow_win+i,y
    jsr .ppu_stripe_write_byte
    i = i + 1
    .endm
    jmp 2$
1$:
.endif
    i = 0
    .rept ATTRIBUTE_PACKED_WIDTH
    lda _attribute_shadow+i,y
    jsr .ppu_stripe_write_byte
    i = i + 1
    .endm
2$:
    jsr .ppu_stripe_end
    ldx *REGTEMP+3
    jmp _flush_shadow_attributes_next_row

;
; Writes every column of attributes from _shadow_attributes that's been marked
; as dirty in the _attribute_column_dirty byte to PPU memory.
;
;
_flush_shadow_attributes_columns:
    lda #<PPU_AT0
    sta *.tmp
    lda .ppu_hi_tab,x
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl 0$
    ora #PPUHI_WIN
0$:
.endif
    sta *.tmp+1
_flush_shadow_attributes_columns_loop:
    lsr *.attribute_column_dirty
    bcc 1$
    jmp _flush_shadow_attributes_update_column
1$:
    beq _flush_shadow_attributes_columns_end
_flush_shadow_attributes_columns_next_column:
    ; Y += 1
    iny
    ; .tmp += 1
    inc *.tmp
    jmp _flush_shadow_attributes_columns_loop
_flush_shadow_attributes_columns_end:
    rts

_write_vert:
    lda *.tmp+1
    tax
    lda *.tmp
    jsr .ppu_stripe_begin_vertical
    lda _attribute_shadow,y
    jsr .ppu_stripe_write_byte
    lda _attribute_shadow+(AT_SHADOW_WIDTH*4),y
    jsr .ppu_stripe_write_byte
    jsr .ppu_stripe_end
    ; inc src index
    tya
    clc
    adc #AT_SHADOW_WIDTH
    tay
    ; inc ppu addr
    lda *.tmp
    adc #ATTRIBUTE_PACKED_WIDTH
    sta *.tmp
    rts

;
; Flushes all dirty rows of _attribute_shadow by writing them to PPU memory
;
_flush_shadow_attributes_update_column:
    stx *REGTEMP+3
    sty *.y_save
    lda *.tmp
    pha
    ; Update all 8 bytes of column for now, as each column in _attribute_column_dirty only stores 1 bit
    ; As PPU has no increment-by-8 feature, split writes into 4 separate stripes 2 bytes each
    ; TODO: Could make a dedicated unrolled transfer routine in nmi handler that writes all 8 bytes as one stripe.
    .rept 4
    jsr _write_vert
    .endm
    pla
    sta *.tmp
    ldy *.y_save
    ldx *REGTEMP+3
    jmp _flush_shadow_attributes_columns_next_column

; Shift MSB of attribute X / Y (attribute table index) from bits 1 and 0 to 7 and 3
.ifdef NES_TILEMAP_F
.xy_shift_tab:
.db 0b00000000
.db 0b00001000
.db 0b10000000
.db 0b10001000
.endif
.ifdef NES_TILEMAP_S
.xy_shift_tab:
.db 0b00000000
.db 0b00000000
.db 0b00000000
.db 0b00000000
.endif
.ifdef NES_TILEMAP_H
.xy_shift_tab:
.db 0b00000000
.db 0b00001000
.endif
.ifdef NES_TILEMAP_V
.xy_shift_tab:
.db 0b00000000
.db 0b01000000
.endif

; Get hi address of attribute table index
.ifdef NES_TILEMAP_F
.ppu_hi_tab:
.db >PPU_AT0
.db >PPU_AT1
.db >PPU_AT2
.db >PPU_AT3
.endif
.ifdef NES_TILEMAP_S
.ppu_hi_tab:
.db >PPU_AT0
.db >PPU_AT0
.db >PPU_AT0
.db >PPU_AT0
.endif
.ifdef NES_TILEMAP_H
.ppu_hi_tab:
.db >PPU_AT0
.db >PPU_AT1
.endif
.ifdef NES_TILEMAP_V
.ppu_hi_tab:
.db >PPU_AT0
.db >PPU_AT2
.endif
