    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    .x_save:                    .ds 1
    .attribute_row_dirty:       .ds 1
    .attribute_column_dirty:    .ds 1

    .area   _HOME

_flush_shadow_attributes::
    ldx #0
.ifndef NES_TILEMAP_S
1$:
.endif
    stx *.x_save
    lda *_attribute_row_dirty,x
    beq 2$
    sta *.attribute_row_dirty
    ldy .xy_shift_tab,x
    jsr _flush_shadow_attributes_rows
    ldx *.x_save
2$:
    lda *_attribute_column_dirty,x
    beq 3$
    sta *.attribute_column_dirty
    ldy .xy_shift_tab,x
    jsr _flush_shadow_attributes_columns
3$:
    ldx *.x_save
    lda #0
    sta _attribute_row_dirty,x
    sta _attribute_column_dirty,x
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
    i = 0
    .rept 8
    lda _attribute_shadow+i,y
    jsr .ppu_stripe_write_byte
    i = i + 1
    .endm
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

.macro WRITEVERT
    lda *.tmp+1
    tax
    lda *.tmp
    clc
    adc #(ATTRIBUTE_PACKED_WIDTH*i)
    jsr .ppu_stripe_begin_vertical
    lda _attribute_shadow+AT_SHADOW_WIDTH*i,y
    jsr .ppu_stripe_write_byte
    lda _attribute_shadow+AT_SHADOW_WIDTH*i+(AT_SHADOW_WIDTH*4),y
    jsr .ppu_stripe_write_byte
    jsr .ppu_stripe_end
.endm

;
; Flushes all dirty rows of _attribute_shadow by writing them to PPU memory
;
_flush_shadow_attributes_update_column:
    stx *REGTEMP+3
    ; Update all 8 bytes of column for now, as each column in _attribute_column_dirty only stores 1 bit
    ; As PPU has no increment-by-8 feature, split writes into 4 separate stripes 2 bytes each
    ; TODO: Could make a dedicated unrolled transfer routine in nmi handler that writes all 8 bytes as one stripe.
    i = 0
    .rept 4
    WRITEVERT
    i = i + 1
    .endm
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
