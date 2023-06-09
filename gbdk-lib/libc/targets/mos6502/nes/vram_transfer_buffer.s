;
; Routines for performing safe buffered writes via the vram transfer buffer.
;
.include    "global.s"

.title  "VRAMBuffer"
.module VRAMBuffer

;
; Format of transfer buffer
;
; 0: Data length
; 1: 4 if inc-by-32, 0 if inc-by-1
; 2: PPUADDR_HI
; 3: PPUADDR_LO
; 4: ...N data bytes...
;
VRAM_HDR_SIZEOF         = 4
VRAM_HDR_LENGTH         = 0
VRAM_HDR_DIRECTION      = 1
VRAM_HDR_PPUHI          = 2
VRAM_HDR_PPULO          = 3
VRAM_MAX_BYTES          = 32
VRAM_MAX_STRIPE_SIZE    = VRAM_HDR_SIZEOF + VRAM_MAX_BYTES

;
; Locks the VRAM buffer for writing, causing the vblank handler to ignore it.
;
.macro VRAM_BUFFER_LOCK
    clc
    ror *__vram_transfer_buffer_valid
.endm

;
; Unlocks the VRAM buffer for writing.
;
.macro VRAM_BUFFER_UNLOCK
    sec
    ror *__vram_transfer_buffer_valid
.endm

.area _ZP (PAG)
__vram_transfer_buffer_valid::          .ds 1
__vram_transfer_buffer_num_cycles_x8::  .ds 1
__vram_transfer_buffer_pos_w::          .ds 1
__vram_transfer_buffer_temp::           .ds 1

.area   _HOME

;
; Begin a horizontal stripe
;
.ppu_stripe_begin_horizontal::
    clc
    jmp .ppu_stripe_begin

;
; Begin a vertical stripe
;
.ppu_stripe_begin_vertical::
    sec
    jmp .ppu_stripe_begin

;
; Begin a stripe (carry indicates vertical stripe)
;
.ppu_stripe_begin::
    bit *.crt0_forced_blanking
    bpl 1$
    ; Direct write
    stx PPUADDR
    sta PPUADDR
    ; Set inc-by-32 bit in PPUCTRL as well
    lda #0
    rol
    asl
    asl
    lda *_shadow_PPUCTRL
    sta PPUCTRL
    rts
1$:
    ; Indirect write via transfer buffer
    sty *__vram_transfer_buffer_temp
    pha
    txa
    pha
    lda #0
    rol
    asl
    asl
    pha
    ; Ensure there's at least VRAM_MAX_STRIPE_SIZE bytes remaining to write before progressing
    ; This conservative limit simplifies conditions for rest of stripe in order to write single bytes with no checks
2$:
    lda *__vram_transfer_buffer_pos_w
    cmp #128-VRAM_MAX_STRIPE_SIZE
    bcs 2$
    ; Lock buffer and store current write pointer for later
    VRAM_BUFFER_LOCK
    ldy *__vram_transfer_buffer_pos_w
    sty *.crt0_textStringBegin
    ; Write direction
    pla
    sta __vram_transfer_buffer+VRAM_HDR_DIRECTION,y
    ; Write address
    pla
    sta __vram_transfer_buffer+VRAM_HDR_PPUHI,y
    pla
    sta __vram_transfer_buffer+VRAM_HDR_PPULO,y
    tya
    clc
    adc #VRAM_HDR_SIZEOF
    sta *__vram_transfer_buffer_pos_w
    ; __vram_transfer_buffer_num_cycles_x8 -= 6 (assumes carry clear)
    lda *__vram_transfer_buffer_num_cycles_x8
    sbc #5
    sta *__vram_transfer_buffer_num_cycles_x8
    ldy *__vram_transfer_buffer_temp
    rts

;
; End a stripe (be it direct or via transfer buffer)
;
.ppu_stripe_end::
    bit *.crt0_forced_blanking
    bpl 1$
    ; For direct writes there's nothing more to do
    rts
1$:
    sty *__vram_transfer_buffer_temp
    ldy *__vram_transfer_buffer_pos_w
    ; Write terminator byte
    lda #0
    sta __vram_transfer_buffer,y
    ; Write number of data bytes
    tya
    sec
    sbc *.crt0_textStringBegin
    sbc #VRAM_HDR_SIZEOF
    ldy *.crt0_textStringBegin
    sta __vram_transfer_buffer+VRAM_HDR_LENGTH,y
    lda *__vram_transfer_buffer_num_cycles_x8
    sbc __vram_transfer_buffer+VRAM_HDR_LENGTH,y
    sta *__vram_transfer_buffer_num_cycles_x8
    VRAM_BUFFER_UNLOCK
    ldy *__vram_transfer_buffer_temp
    rts

;
; Writes a byte of an in-progress horizontal / vertical stripe (be it direct or via transfer buffer)
;
.ppu_stripe_write_byte::
    bit *.crt0_forced_blanking
    bpl 1$
    sta PPUDATA     ; Direct write
    rts
1$:
    sty *__vram_transfer_buffer_temp
    ldy *__vram_transfer_buffer_pos_w
    sta __vram_transfer_buffer,y
    inc *__vram_transfer_buffer_pos_w
    ldy *__vram_transfer_buffer_temp
    rts

.writeNametableByte::
_writeNametableByte::
    pha
    lda #(0x20 >> 5)
    sta *__crt0_textPPUAddr+1
    tya
    asl
    rol *__crt0_textPPUAddr+1
    asl
    rol *__crt0_textPPUAddr+1
    asl
    rol *__crt0_textPPUAddr+1
    asl
    rol *__crt0_textPPUAddr+1
    asl
    rol *__crt0_textPPUAddr+1
    ora .identity,x
    sta *__crt0_textPPUAddr
    ; If we're currently in forced blanking mode, use direct writes instead of transfer buffer
    bit *.crt0_forced_blanking
    bpl 1$
    lda *__crt0_textPPUAddr+1
    sta PPUADDR
    lda *__crt0_textPPUAddr
    sta PPUADDR
    lda #0
    rol
    asl
    asl
    ora *_shadow_PPUCTRL
    sta PPUCTRL
    pla
    sta PPUDATA
    ; Increment PPU addr
    lda *__crt0_textPPUAddr
    clc
    adc #1
    sta *__crt0_textPPUAddr
    lda *(__crt0_textPPUAddr+1)
    adc #0
    sta *(__crt0_textPPUAddr+1)
    rts
1$:
_writeNametableByteWaitForFlush:
    ldy *__vram_transfer_buffer_pos_w
    tya
    ; Limit when (slightly more than) equal to 80 bytes, to avoid overrunning vblank time
    cmp #80
    bcs _writeNametableByteWaitForFlush
    VRAM_BUFFER_LOCK
    ; First check if we can append to previously added stripe
    ldy *.crt0_textStringBegin
    ; Only append if current vram buffer pointer directly follows previous stripe
    tya
    clc
    adc __vram_transfer_buffer+VRAM_HDR_LENGTH,y
    adc #VRAM_HDR_SIZEOF
    cmp *__vram_transfer_buffer_pos_w
    bne 2$
    ; Only append if previous stripe was horizontal
    lda __vram_transfer_buffer+VRAM_HDR_DIRECTION,y
    bne 2$
    ; Only append if PPU address following previous stripe matches desired PPU address
    lda __vram_transfer_buffer+VRAM_HDR_PPULO,y
    clc
    adc __vram_transfer_buffer+VRAM_HDR_LENGTH,y
    sta *.tmp
    lda __vram_transfer_buffer+VRAM_HDR_PPUHI,y
    adc #0
    ; Check against textPPUAddr
    cmp *__crt0_textPPUAddr+1
    bne 2$
    lda *.tmp
    cmp *__crt0_textPPUAddr
    bne 2$
    jmp _writeNametableByte_continue
2$:
    jmp _writeNametableByte_new

_writeNametableByte_end:
    ; Increment PPU addr
    lda *__crt0_textPPUAddr
    clc
    adc #1
    sta *__crt0_textPPUAddr
    lda *(__crt0_textPPUAddr+1)
    adc #0
    sta *(__crt0_textPPUAddr+1)
    sty *__vram_transfer_buffer_pos_w
    VRAM_BUFFER_UNLOCK
    ldy *__vram_transfer_buffer_temp
    rts

;
; Writes a new nametable byte, effectively beginning a new stripe
;
_writeNametableByte_new:
    ldy *__vram_transfer_buffer_pos_w
    sty *.crt0_textStringBegin
    ; Number of bytes
    lda #1
    sta __vram_transfer_buffer,y
    iny
    ; Horizontal mode
    lda #0
    sta __vram_transfer_buffer,y
    iny
    ; PPU hi
    lda *(__crt0_textPPUAddr+1)
    sta __vram_transfer_buffer,y
    iny
    ; PPU lo
    lda *__crt0_textPPUAddr
    sta __vram_transfer_buffer,y
    iny
    pla
    sta __vram_transfer_buffer,y
    iny
    ; zero byte at end
    lda #0
    sta __vram_transfer_buffer,y
    ; decrease total delay in NMI by 7 * 8 cycles for each individually written byte
    ;
    ; Each new write operation adds 48 cycles + 8 cycles per written byte, i.e.:
    ; _cycles_x8 -= 6 * numberOfWriteCommands + numWrittenBytes
    ;
    ; __vram_transfer_buffer_num_cycles_x8 -= 7
    lda *__vram_transfer_buffer_num_cycles_x8
    sec
    sbc #7
    sta *__vram_transfer_buffer_num_cycles_x8
    jmp _writeNametableByte_end

;
; Continues the previously started stripe by adding an additional byte to it
;
_writeNametableByte_continue:
    ; Increase number of bytes in header
    ldy *.crt0_textStringBegin
    lda __vram_transfer_buffer,y
    clc
    adc #1
    sta __vram_transfer_buffer,y
    ; Write new byte
    ldy *__vram_transfer_buffer_pos_w
    pla
    sta __vram_transfer_buffer,y
    iny
    ; zero byte at end
    lda #0
    sta __vram_transfer_buffer,y
    ; Decrease x8 cycle delay by 1
    dec *__vram_transfer_buffer_num_cycles_x8
    jmp _writeNametableByte_end
