    .include    "global.s"

    .area   _HOME
    
_flush_shadow_attributes::
    jsr _flush_shadow_attributes_rows
    jmp _flush_shadow_attributes_columns

;
; Writes every row of attributes from _shadow_attributes that's been marked
; as dirty in the _attribute_row_dirty byte to PPU memory.
;
_flush_shadow_attributes_rows:
    lda #<PPU_AT0
    sta *.tmp
    lda #>PPU_AT0
    sta *.tmp+1
    ldy #0
_flush_shadow_attributes_row_loop:
    lsr *_attribute_row_dirty
    bcc 1$
    jmp _flush_shadow_attributes_update_row
1$:
    beq _flush_shadow_attributes_end
_flush_shadow_attributes_next_row:
    ; Y += 8
    tya
    clc
    adc #8
    tay
    ; .tmp += 8
    lda *.tmp
    adc #8
    sta *.tmp
    jmp _flush_shadow_attributes_row_loop
_flush_shadow_attributes_end:
    rts

;
; Flushes all dirty rows of _attribute_shadow by writing them to PPU memory
;
_flush_shadow_attributes_update_row:
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
    jmp _flush_shadow_attributes_next_row

;
; Writes every column of attributes from _shadow_attributes that's been marked
; as dirty in the _attribute_column_dirty byte to PPU memory.
;
;
_flush_shadow_attributes_columns:
    lda #<PPU_AT0
    sta *.tmp
    lda #>PPU_AT0
    sta *.tmp+1
    ldy #0
_flush_shadow_attributes_columns_loop:
    lsr *_attribute_column_dirty
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
    adc #(8*i)
    jsr .ppu_stripe_begin_vertical
    lda _attribute_shadow+8*i,y
    jsr .ppu_stripe_write_byte
    lda _attribute_shadow+8*i+32,y
    jsr .ppu_stripe_write_byte
    jsr .ppu_stripe_end
.endm

;
; Flushes all dirty rows of _attribute_shadow by writing them to PPU memory
;
_flush_shadow_attributes_update_column:
    ; Update all 8 bytes of column for now, as each column in _attribute_column_dirty only stores 1 bit
    ; As PPU has no increment-by-8 feature, split writes into 4 separate stripes 2 bytes each
    ; TODO: Could make a dedicated unrolled transfer routine in nmi handler that writes all 8 bytes as one stripe.
    i = 0
    .rept 4
    WRITEVERT
    i = i + 1
    .endm
    jmp _flush_shadow_attributes_columns_next_column
