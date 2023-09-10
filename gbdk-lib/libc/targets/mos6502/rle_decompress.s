    .include "global.s"

    .module RLE_DECOMPRESS

    .area _ZP (PAG)
rle_cursor:
        .ds 0x02
rle_counter:
        .ds 0x01
rle_current:
        .ds 0x01
rle_code_ptr:
        .ds 0x02

    .area   OSEG (PAG, OVR)
_rle_decompress_PARM_2::        .ds 1
.rle_output:                   .ds 2

    .define .out_len "_rle_decompress_PARM_2"

    .area   _HOME

.macro RLE_ADVANCE_CURSOR
    tya
    clc
    adc *rle_cursor
    sta *rle_cursor
    lda #0
    adc *rle_cursor+1
    sta *rle_cursor+1
.endm

.macro RLE_ADVANCE_OUTPUT
    tya
    clc
    adc *.rle_output
    sta *.rle_output
    lda #0
    adc *.rle_output+1
    sta *.rle_output+1
.endm

.macro RLE_SET_CODE_PTR code_address
    lda #<code_address
    sta *rle_code_ptr
    lda #>code_address
    sta *(rle_code_ptr+1)
.endm

_rle_init::
    sta *rle_cursor
    stx *(rle_cursor+1)
    RLE_SET_CODE_PTR _rle_decompress_read_token_entry
    lda	#0x01
    rts

_rle_decompress_eof:
    RLE_SET_CODE_PTR _rle_decompress_eof
    lda #0
    rts

_rle_decompress::
    jmp [*rle_code_ptr]
    
_rle_decompress_read_token_entry:
    sta *.rle_output
    stx *(.rle_output+1)
    ldx *.out_len
    ldy #0
_rle_decompress_read_token:
    lda [*rle_cursor],y
    beq _rle_decompress_eof
    bmi _rle_decompress_read_token_repeat
    ; RLE_TYPE_RAND
    sta *rle_counter
    RLE_SET_CODE_PTR _rle_decompress_process_rand_entry
    inc *rle_cursor
    beq 1$
    jmp _rle_decompress_process_rand
1$:
    inc *rle_cursor+1
    jmp _rle_decompress_process_rand
_rle_decompress_read_token_repeat:
    ; RLE_TYPE_REPEAT
    sta *rle_counter
    RLE_SET_CODE_PTR _rle_decompress_process_repeat_entry
    inc *rle_cursor
    beq 2$
    lda [*rle_cursor],y
    sta *rle_current
    inc *rle_cursor
    bne 1$
    inc *(rle_cursor+1)
1$:
    jmp _rle_decompress_process_repeat
2$:
    inc *(rle_cursor+1)
    lda [*rle_cursor],y
    sta *rle_current
    inc *rle_cursor
    jmp _rle_decompress_process_repeat

_rle_decompress_process_rand_entry:
    sta *.rle_output
    stx *(.rle_output+1)
    ldx *.out_len
    ldy #0
_rle_decompress_process_rand:
1$:
    cpx #0
    beq _rle_decompress_process_rand_yield
    lda [*rle_cursor],y
    sta [*.rle_output],y
    dex
    iny
    dec *rle_counter
    bne 1$
    RLE_ADVANCE_CURSOR
    RLE_ADVANCE_OUTPUT
    ldy #0
    jmp _rle_decompress_read_token

_rle_decompress_process_rand_yield:
    RLE_ADVANCE_CURSOR
    lda #1
    rts

_rle_decompress_process_repeat_entry:
    sta *.rle_output
    stx *(.rle_output+1)
    ldx *.out_len
    ldy #0
_rle_decompress_process_repeat:
    lda *rle_current
1$:
    cpx #0
    beq _rle_decompress_process_repeat_yield
    sta [*.rle_output],y
    dex
    iny
    inc *rle_counter
    bne 1$
    RLE_ADVANCE_OUTPUT
    ldy #0
    jmp _rle_decompress_read_token

_rle_decompress_process_repeat_yield:
    lda #1
    rts
