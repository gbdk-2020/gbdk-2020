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

.area   GBDKOVR (PAG, OVR)
_set_vram_byte_PARM_2:: .ds 1
    
.area _ZP (PAG)
__vram_transfer_buffer_valid::          .ds 1
__vram_transfer_buffer_num_cycles_x8::  .ds 1
__vram_transfer_buffer_pos_w::          .ds 1
__vram_transfer_buffer_pos_old::        .ds 1

.define __vram_transfer_buffer_temp     "(REGTEMP+6)"

.area   _HOME

;
; Begin a horizontal stripe
;
.ppu_stripe_begin_horizontal::
    clc
    bit *.crt0_forced_blanking
    bpl .ppu_stripe_begin_indirect
    ; Direct write
    stx PPUADDR
    sta PPUADDR
    ; Clear inc-by-32 bit in both PPUCTRL and _shadow_PPUCTRL, as NMI may re-write PPUCTRL
    lda *_shadow_PPUCTRL
    and #0xFB
    sta *_shadow_PPUCTRL
    sta PPUCTRL
    rts

;
; Begin a vertical stripe
;
.ppu_stripe_begin_vertical::
    sec
    bit *.crt0_forced_blanking
    bpl .ppu_stripe_begin_indirect
    ; Direct write
    stx PPUADDR
    sta PPUADDR
    ; Set inc-by-32 bit in both PPUCTRL and _shadow_PPUCTRL, as NMI may re-write PPUCTRL
    lda *_shadow_PPUCTRL
    ora #0x04
    sta *_shadow_PPUCTRL
    sta PPUCTRL
    rts

.ppu_stripe_begin_indirect:
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
    sty *__vram_transfer_buffer_pos_old
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
.ppu_stripe_end_indirect:
    sty *__vram_transfer_buffer_temp
    ldy *__vram_transfer_buffer_pos_w
    ; Write terminator byte
    lda #0
    sta __vram_transfer_buffer,y
    ; Write number of data bytes
    tya
    sec
    sbc *__vram_transfer_buffer_pos_old
    sbc #VRAM_HDR_SIZEOF
    ldy *__vram_transfer_buffer_pos_old
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
.ppu_stripe_write_byte_indirect:
    sty *__vram_transfer_buffer_temp
    ldy *__vram_transfer_buffer_pos_w
    sta __vram_transfer_buffer,y
    inc *__vram_transfer_buffer_pos_w
    ldy *__vram_transfer_buffer_temp
    rts

;
; Appends a single byte to the last finished stripe when possible, temporarily re-opening it.
;
; The last stripe will only be re-used if it is possible to do so. 
; The following conditions must be true:
; - The transfer buffer is not empty (a "last stripe" must exist in buffer)
; - The last stripe's number of bytes isn't already VRAM_MAX_BYTES number of bytes
; - The last stripe is either horizontal or single-byte, and the new byte's address is last_address+1
; - The last stripe is either vertical or single-byte, and the next byte's address is last_address+32
;
; If any of these conditions aren't true, a new stripe will be created instead.
;
; The current version has a further simplification: The high byte of PPU address must be unchanged.
; This simplification is there to avoid doing exhaustive and slow 16-bit comparisons.
; This means a vertical stripe which is automatically appended to by this code can be no longer than 8 bytes.
;
_set_vram_byte::
.ppu_stripe_append::
    .define ppu_addr ".tmp"
    ;
    bit *.crt0_forced_blanking
    bpl .ppu_stripe_append_indirect
    ; Direct write
    stx PPUADDR
    sta PPUADDR
    ldy *_set_vram_byte_PARM_2
    sty PPUDATA
    rts

.ppu_stripe_wait_for_flush:
    ; Unlock and wait for flush by NMI handler
    VRAM_BUFFER_UNLOCK
1$:
    ldx *__vram_transfer_buffer_pos_w
    bmi 1$
    ; Re-lock buffer and jump straight to new-stripe code
    VRAM_BUFFER_LOCK
    jmp .ppu_stripe_append_failed
    
.ppu_stripe_append_indirect:
    ; Indirect write via transfer buffer
    sta *ppu_addr
    stx *ppu_addr+1
    ; Lock buffer first, to make sure our checks don't get invalidated by NMI
    VRAM_BUFFER_LOCK
    ; Now that buffer is safely locked, first check if it's almost full
    ldx *__vram_transfer_buffer_pos_w
    bmi .ppu_stripe_wait_for_flush
    ; check that it's not empty
    beq .ppu_stripe_append_failed
    ldx *__vram_transfer_buffer_pos_old
    ; Don't handle appending when high address changed
    lda __vram_transfer_buffer+VRAM_HDR_PPUHI,x
    cmp *ppu_addr+1
    bne .ppu_stripe_append_failed
    lda __vram_transfer_buffer+VRAM_HDR_LENGTH,x
    cmp #VRAM_MAX_BYTES
    beq .ppu_stripe_append_failed
    ; if last stripe only contains a single byte, branch to go-either-direction routine
    cmp #1
    beq .ppu_stripe_append_second_byte
    ; Test for horizontal stripe
    lda __vram_transfer_buffer+VRAM_HDR_DIRECTION,x
    bne 2$
    ; Horizontal stripe - check if new address is 1*LENGTH
    lda *ppu_addr
    sec
    sbc __vram_transfer_buffer+VRAM_HDR_PPULO,x
    cmp __vram_transfer_buffer+VRAM_HDR_LENGTH,x
    beq 0$
    jmp .ppu_stripe_append_failed
0$:
    ; Append byte
    jmp .ppu_stripe_append_1byte
2$:
    ; Vertical stripe - check if new address is 32*LENGTH
    lda *ppu_addr
    lsr
    lsr
    lsr
    lsr
    lsr
    sec
    sbc __vram_transfer_buffer+VRAM_HDR_PPULO,x
    cmp __vram_transfer_buffer+VRAM_HDR_LENGTH,x
    bne .ppu_stripe_append_failed
    ; Append 1 byte
    jmp .ppu_stripe_append_1byte

.ppu_stripe_append_failed:
    ; Just create a new stripe with a single byte
    ldy *__vram_transfer_buffer_pos_w
    sty *__vram_transfer_buffer_pos_old
    ; Write direction (assume horizontal) and new terminator byte
    lda #0
    sta __vram_transfer_buffer+VRAM_HDR_DIRECTION,y
    sta __vram_transfer_buffer+VRAM_HDR_SIZEOF+1,y
    ; Write length
    lda #1
    sta __vram_transfer_buffer+VRAM_HDR_LENGTH,y
    ; Write address
    lda *ppu_addr+1
    sta __vram_transfer_buffer+VRAM_HDR_PPUHI,y
    lda *ppu_addr
    sta __vram_transfer_buffer+VRAM_HDR_PPULO,y
    ; write data byte
    lda *_set_vram_byte_PARM_2
    sta __vram_transfer_buffer+VRAM_HDR_SIZEOF,y
    ; Increase write pointer
    tya
    clc
    adc #VRAM_HDR_SIZEOF+1
    ; store new write pointer
    sta *__vram_transfer_buffer_pos_w
    ; __vram_transfer_buffer_num_cycles_x8 -= 7 (assumes carry clear)
    lda *__vram_transfer_buffer_num_cycles_x8
    sbc #6
    sta *__vram_transfer_buffer_num_cycles_x8
    VRAM_BUFFER_UNLOCK
    ; Return PPU address
    lda *ppu_addr
    ldx *ppu_addr+1
    rts

.ppu_stripe_append_second_byte:
    ; We only have one previous byte, so ppu_addr being +1 or +32 would both work
    lda *ppu_addr
    sec
    sbc __vram_transfer_buffer+VRAM_HDR_PPULO,x
    cmp #1
    bne 1$
    ; Convert to horizontal stripe
    lda #0
    sta __vram_transfer_buffer+VRAM_HDR_DIRECTION,x
    jmp .ppu_stripe_append_1byte
1$:
    cmp #32
    bne 2$
    ; Convert to vertical stripe
    lda #4
    sta __vram_transfer_buffer+VRAM_HDR_DIRECTION,x
    jmp .ppu_stripe_append_1byte
2$:
    jmp .ppu_stripe_append_failed

.ppu_stripe_append_1byte:
    ; Append 1 byte
    inc __vram_transfer_buffer+VRAM_HDR_LENGTH,x
    lda *_set_vram_byte_PARM_2
    ldx *__vram_transfer_buffer_pos_w
    sta __vram_transfer_buffer,x
    inx
    ; Write terminator byte
    lda #0
    sta __vram_transfer_buffer,x
    stx *__vram_transfer_buffer_pos_w
    ; Decrease x8 cycle delay by 1
    dec *__vram_transfer_buffer_num_cycles_x8
    ; unlock buffer
    VRAM_BUFFER_UNLOCK
    ; Return PPU address
    lda *ppu_addr
    ldx *ppu_addr+1
    rts
