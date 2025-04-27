;
; crt0.s for NES, using UNROM-512 (mapper30) with single-screen mirroring variant
;
; Provides:
;  * Start-up code clearing RAM and VRAM
;  * Constant-cycle-time NMI handler, performing sprite DMA and VRAM writes via transfer buffer at $100
;  * 16-bit frame counter _sys_time, to support VM routines
.module crt0
.include    "global.s"

; OAM CPU page
_shadow_OAM             = 0x200
; Attribute shadow (64 bytes, leaving 56 bytes available for CPU stack)
_attribute_shadow       = 0x188

.macro WRITE_PALETTE_SHADOW
    lda #>0x3F00
    sta PPUADDR
    lda #<0x3F00
    sta PPUADDR
    ldx __crt0_paletteShadow
    i = 0
.rept 8
    stx PPUDATA
    lda (__crt0_paletteShadow+1+3*i+0)
    sta PPUDATA
    lda (__crt0_paletteShadow+1+3*i+1)
    sta PPUDATA
    lda (__crt0_paletteShadow+1+3*i+2)
    sta PPUDATA
    i = i + 1
.endm
.endm

       ;; ****************************************

        ;; Ordering of segments for the linker
        ;; Code that really needs to be in the fixed bank
        .area _CODE
        .area _HOME
        ;; Similar to _HOME
        .area _BASE
        ;; Constant data
        .area _LIT
        .area _RODATA
        ;; Constant data, used to init _DATA
        .area _INITIALIZER
        .area _XINIT
        ;; Code, used to init _DATA
        .area _GSINIT 
        .area _GSFINAL
        ;; Uninitialised ram data
        .area _DATA
        .area _BSS
        ;; Initialised in ram data
        .area _INITIALIZED
        ;; For malloc
        .area _HEAP
        .area _HEAP_END

.area OSEG (PAG, OVR)
.area GBDKOVR (PAG, OVR)
.area _ZP (PAG)
__shadow_OAM_base::                     .ds 1
__current_bank::                        .ds 1
_sys_time::                             .ds 2
_shadow_PPUCTRL::                       .ds 1
_shadow_PPUMASK::                       .ds 1
_bkg_scroll_x::                         .ds 1
_bkg_scroll_y::                         .ds 1
_attribute_row_dirty::                  .ds 1
_attribute_column_dirty::               .ds 1
__oam_valid_display_on::                .ds 1
__SYSTEM::                              .ds 1
__hblank_writes_index::                 .ds 1

.define __crt0_NMITEMP "___SDCC_m6502_ret4"

.area _DATA
_TIMA_REG::                             .ds 1
_TMA_REG::                              .ds 1

.area _XINIT
.db 0xFF
.db 0xFF

.area _BSS
__crt0_paletteShadow::                  .ds 25
.mode::                                 .ds 1
__lcd_isr_PPUCTRL::                     .ds (2*.MAX_DEFERRED_ISR_CALLS)
__lcd_isr_PPUMASK::                     .ds (2*.MAX_DEFERRED_ISR_CALLS)
__lcd_isr_scroll_x::                    .ds (2*.MAX_DEFERRED_ISR_CALLS)
__lcd_isr_scroll_y::                    .ds (2*.MAX_DEFERRED_ISR_CALLS)
__lcd_isr_delay_num_scanlines::         .ds (2*.MAX_DEFERRED_ISR_CALLS)
__lcd_isr_ppuaddr_lo::                  .ds (2*.MAX_DEFERRED_ISR_CALLS)
_TAC_REG::                              .ds 1 ; Unused, for GB compatibility

.area _CODE

.bndry 0x100
.identity::
_identity::
i = 0
.rept 256
.db i
i = i + 1
.endm

.define ProcessDrawList_tempX "__crt0_NMITEMP+2"
.define ProcessDrawList_addr  "__crt0_NMITEMP+0"

.bndry 0x100
    nop         ; Pad to offset, to support zero-terminator value
ProcessDrawList_UnrolledCopyLoop:
.rept 32
pla             ; +4
sta PPUDATA     ; +4
.endm
ProcessDrawList_DoOneTransfer:
    pla                                         ; +4
    beq ProcessDrawList_EndOfList               ; +2/3
    sta *ProcessDrawList_addr                   ; +3
    pla                                         ; +4
    sta PPUCTRL                                 ; +4
    pla                                         ; +4
    sta PPUADDR                                 ; +4
    pla                                         ; +4
    sta PPUADDR                                 ; +4
    nop                                         ; +2
    jmp [ProcessDrawList_addr]                  ; +5
    ; Total = 4 + 2 + 2 + 4 + 3 + 6*4 + 2 + 2 + 5 = 48 for each transfer (...+ 8*NumBytesCopied)
    ;         4 + 3 + 14 = 7 + 14 = 21 fixed-cost exit

; .bndry 0x100 (skip alignment as previous alignment means page-cross won't happen)
__crt0_doSpriteDMA:
    bit *__oam_valid_display_on
    bpl __crt0_doSpriteDMA_spritePageInvalid
    lda #0                      ; +2
    sta OAMADDR                 ; +4
    lda #>_shadow_OAM           ; +2
    sta OAMDMA                  ; +512/513
    rts
__crt0_doSpriteDMA_spritePageInvalid:
    ; Delay 520 cycles to keep timing consistent
    ldx #104
__crt0_doSpriteDMA_loop:
    dex
    bne __crt0_doSpriteDMA_loop
    rts

ProcessDrawList_EndOfList:
    ldx *ProcessDrawList_tempX          ; +3
    txs                                 ; +2
    lda #0                              ; +2
    sta *__vram_transfer_buffer_pos_w   ; +3
    sta *__vram_transfer_buffer_valid   ; +3
    rts                                 ; +6
    ; = 3 + 2 + 2 + 3 + 3 + 6 = 19

;
; Number of cycles spent = 19 + 21 + 48*NumTransfers + 8*NumBytesTransferred
;                        = 56 + 48*NumTransfers + 8*NumBytesTransferred
;                        = 8 * (7 + 6*NumTransfers + NumBytesTransferred)
;                        = 8 * (6*NumTransfers + NumBytesTransferred + 7)
;
ProcessDrawList:
    lda #>ProcessDrawList_UnrolledCopyLoop  ; +2
    sta *ProcessDrawList_addr+1             ; +3
    tsx                                     ; +2
    stx *ProcessDrawList_tempX              ; +3
    ldx #0xFF                               ; +2
    txs                                     ; +2
    jmp ProcessDrawList_DoOneTransfer       ; +3
    ; Total = 2 + 3 + 2 + 3 + 2 + 2 + 3 = 17 fixed-cost entry

;
; Delays until specified (non-zero) scanline is reached
;
; First scanline's delay needs adjusting in coordination with .do_hblank_writes
;
.define .acc "___SDCC_m6502_ret4"
.delay_to_lcd_scanline::
    jmp 2$
1$:
    jsr .delay_28_cycles
    jsr .delay_28_cycles
    jsr .delay_12_cycles ; -> 28 + 28 + 12 = 68 cycles
    clc
2$:

    jsr .delay_fractional   ; -> 35.666 NTSC cycles  28.5625 PAL cycles
    lda *0x00

    dex
    bne 1$      ; -> 5 cycles
    rts

.delay_28_cycles:
    jsr .delay_12_cycles
.delay_16_cycles:
    nop
.delay_14_cycles:
    nop
.delay_12_cycles:
    rts

;
; Takes 35.666 NTSC cycles / 28.5626 PAL cycles
;
; Note: does NOT clear carry - this needs to be handled by caller
;
.delay_fractional:
    lda #144 ; Initialize A with PAL fractional cycle count
    ; +7 cycles for NTSC scanlines
    bit *__SYSTEM
    bvs 3$
    lda #171 ; NTSC fractional cycle count
    nop
    nop
    nop
3$:             ; -> 15 NTSC cycles / 8 PAL cycles
    ; Add fractional cycles and branch on carry
    adc *.acc
    bcs 4$
4$:
    sta *.acc   ; -> 8.666 NTSC cycles / 8.5625 PAL cycles
    rts         ; -> 6 cycles for RTS, 6 cycles for JSR = 12 cycles

__crt0_NMI:
    pha
    txa
    pha
    tya
    pha
    
    ; Skip graphics updates if blanked, to allow main code to do VRAM address / scroll updates
    nop
    bit *__oam_valid_display_on
    bvs __crt0_NMI_skip
    ; Do Sprite DMA or delay equivalent cycles
    jsr __crt0_doSpriteDMA
    ; Update VRAM
    lda PPUSTATUS
    lda #PPUCTRL_SPR_CHR
    sta PPUCTRL
    jsr DoUpdateVRAM

    ; First index is from main code / VBL ISR
    ldy *__hblank_writes_index
    ; Set scroll address
    lda __lcd_isr_scroll_x,y
    sta PPUSCROLL
    lda __lcd_isr_scroll_y,y
    sta PPUSCROLL
  
    ; Write PPUCTRL and force NMI enabled to avoid deadlock from buggy isr handlers
    lda __lcd_isr_PPUCTRL,y
    ora #0x80
    sta PPUCTRL

    ; Write PPUMASK, in case it was disabled
    lda __lcd_isr_PPUMASK,y
    sta PPUMASK

    ; Delay and call fake LCD isr if list not empty
    lda __lcd_isr_delay_num_scanlines+1,y   ; Check for first potential LCD write
    beq __crt0_NMI_skip
    ; First delay until end-of-vblank, depending on transfer buffer contents...
    ; (X set to correct delay value by DoUpdateVRAM)
1$:
    lda *0x00
    dex
    bne 1$
    ; Do additional delay of 5186 cycles if running on a PAL system, and -5*7 + 2 = -33 for alignment
    ; This is to compensate for the longer vblank period of 7459 vs NTSC's 2273
    bit *__SYSTEM
    bvc 2$
    nop
    nop
    ldy #5
    ldx #(14-7)
3$:
    dex
    bne 3$
    dey
    bne 3$
2$:
    ldy *__hblank_writes_index
    iny   ; Move index past VBL write, to first potential LCD write
    ; Call the write reg subroutine
    jsr .do_hblank_writes
__crt0_NMI_skip:

    ; Call the timer interrupt for non-graphics events if timer register overflow, and reload TIMA_REG
    inc _TIMA_REG
    bne 3$
    ; Skip if disabled (check for RTS)
    lda .jmp_to_TIM_isr
    cmp #0x60
    beq 6$
    ; Save REGTEMP to stack
    ldx #7
4$:
    lda *REGTEMP,x
    pha
    dex
    bpl 4$
    ; Call handler
    jsr .jmp_to_TIM_isr
    ; Restore REGTEMP from stack
    ldx #0
5$:
    pla
    sta *REGTEMP,x
    inx
    cpx #8
    bne 5$
    ;
6$:
    lda _TMA_REG
    sta _TIMA_REG
3$:

    ; Update frame counter
    lda *_sys_time
    clc
    adc #1
    sta *_sys_time
    bcc 9$
    inc *(_sys_time+1)
9$:

    pla
    tay
    pla
    tax
    pla
    rti

DoUpdateVRAM:
    WRITE_PALETTE_SHADOW
    bit *__vram_transfer_buffer_valid
    bmi DoUpdateVRAM_drawListValid
DoUpdateVRAM_drawListInvalid:
    ; Delay for all unused cycles and ProcessDrawList overhead to keep timing consistent
    lda *0x00
    nop
    ldx #(VRAM_DELAY_CYCLES_X8+6)
    bne DoUpdateVRAM_end
DoUpdateVRAM_drawListValid:
    jsr ProcessDrawList
    ; Delay for remaining unused cycles to keep timing consistent
    ldx *__vram_transfer_buffer_num_cycles_x8
    ; Reset available cycles to initial value
    lda #VRAM_DELAY_CYCLES_X8
    sta *__vram_transfer_buffer_num_cycles_x8
DoUpdateVRAM_end:
    rts

__crt0_IRQ:
    jmp __crt0_IRQ

__crt0_setPalette:
    ; Set background color to 30 (white)
    lda #0x30
    sta __crt0_paletteShadow
    ; set all background / sprite sub-palettes to 10, 00, 1D
    ldx #0x18
1$:
    lda #0x1D
    sta __crt0_paletteShadow,x
    dex
    lda #0x00
    sta __crt0_paletteShadow,x
    dex
    lda #0x10
    sta __crt0_paletteShadow,x
    dex
    bne 1$
    rts

;
; Waits for vblank flag to be set. This macro should only be
; used during the PPU warm-up phase at reset, as a hardware 
; flaw can cause the flag to be cleared in the register
; without returning a set flag on the CPU data bus.
;
; On Dendy-like Famiclones there is an additional problem
; that this pathological case can occur *every* frame if the wait
; loop is exactly 8 cycles long, causing a soft-lock at reset.
; For this reason, code before the "bpl .loop" instruction must 
; be aligned so the branch does not cross a 256-byte page.
;
; https://www.nesdev.org/wiki/PPU_power_up_state
;
.macro CRT0_WAIT_PPU ?.loop;
.loop:
    lda PPUSTATUS
    bpl .loop
.endm

;
; Detects system. After execution, A contains the following values:
;
; 0: NTSC NES/Famicom
; 1: PAL NES
; 2: Dendy-like Famiclone
;
.macro CRT0_WAIT_PPU_AND_DETECT_SYSTEM ?.loop, ?.end_of_loop, ?.end
    ldx #0
    ldy #0
; 256 iterations of the inner loop (X) takes 256 * (4 + 2 + 2 + 3) - 1 = 2816 cycles
; 1 iteration of the outer loop takes 2816 + 2 + 3 = 2821 cycles
; And different systems will have the following contents in Y:
; NTSC:   29780 / 2821 = 10
; PAL:    33247 / 2821 = 11
; Dendy:  35464 / 2821 = 12
.loop:
    bit PPUSTATUS
    bmi .end_of_loop
    inx
    bne .loop
    iny
    bne .loop
.end_of_loop:
    tya
    sec
    sbc #10
.end:
.endm

.macro CRT0_CLEAR_RAM
    ldx #0x00
    txa
__crt0_clearRAM_loop:
    sta 0x0000,x
    sta 0x0100,x
    sta 0x0200,x
    sta 0x0300,x
    sta 0x0400,x
    sta 0x0500,x
    sta 0x0600,x
    sta 0x0700,x
    inx
    bne __crt0_clearRAM_loop
.endm

__crt0_clearVRAM:
    lda #0x00
    sta PPUADDR
    sta PPUADDR
    ldy #64
    ldx #0
__crt0_clearVRAM_loop:
    sta PPUDATA
    dex
    bne __crt0_clearVRAM_loop
    dey
    bne __crt0_clearVRAM_loop
    rts

.wait_vbl_done::
_wait_vbl_done::
_vsync::
    jsr _flush_shadow_attributes

    ; if display is on, run deferred ISR handlers
    bit *__oam_valid_display_on
    bvs 1$
    jsr .deferred_isr_run
1$:
    ; Enable OAM DMA in next NMI
    lda *__oam_valid_display_on
    ora #OAM_VALID_MASK
    sta *__oam_valid_display_on

_wait_vbl_done_waitForNextFrame:
    lda *_sys_time
_wait_vbl_done_waitForNextFrame_loop:
    cmp *_sys_time
    beq _wait_vbl_done_waitForNextFrame_loop

    ; Disable OAM DMA in next NMI
    lda *__oam_valid_display_on
    and #~OAM_VALID_MASK
    sta *__oam_valid_display_on

    rts

.display_off::
_display_off::
    ; Skip entirely if display_off is called repeatedly
    bit *__oam_valid_display_on
    bvs 1$
    ; Reset deferred ISR buffers
    jsr .deferred_isr_reset
    ; Clear BG and SPR in first __lcd_isr_PPUMASK, to cause 
    ; NMI code to clear PPUMASK *after* 1 normal execution
    lda *_shadow_PPUMASK
    and #~(PPUMASK_SHOW_BG|PPUMASK_SHOW_SPR)
    sta __lcd_isr_PPUMASK
    ; Wait for 1 execution of NMI to drain vram transfer buffer
    jsr _wait_vbl_done_waitForNextFrame
    ; Set forced blanking bit
    lda *__oam_valid_display_on
    ora #DISPLAY_OFF_MASK
    sta *__oam_valid_display_on
1$:
    rts

.display_on::
_display_on::
    ; Skip entirely if display_on is called repeatedly
    bit *__oam_valid_display_on
    bvc 1$
    ; Reset deferred ISR buffers
    jsr .deferred_isr_reset_and_init
    ; Set DISPLAY_ON bits
    lda *__oam_valid_display_on
    and #~DISPLAY_OFF_MASK
    sta *__oam_valid_display_on
1$:
    rts

__crt0_RESET:
    ; Disable IRQs
    sei
    ; Set stack pointer
    ldx #0xff
    txs
    ; Set switchable bank to first
__crt0_RESET_bankSwitchValue:
    lda #0x00
    sta __crt0_RESET_bankSwitchValue+1
    ; Disable NMIs and rendering
    sta PPUCTRL
    sta PPUMASK
    ; Clear RAM
    CRT0_CLEAR_RAM
    ; Wait for PPU warm-up / detect system
    bit PPUSTATUS
    CRT0_WAIT_PPU
    CRT0_WAIT_PPU_AND_DETECT_SYSTEM
    ; Store system in upper two bits of __SYSTEM, to allow bit instruction to quickly test for PAL
    clc
    ror
    ror
    ror
    sta *__SYSTEM
    ; Clear VRAM
    jsr __crt0_clearVRAM
    ; Hide sprites in shadow OAM, and perform OAM DMA
    ldx #0
    txa
    jsr _hide_sprites_range
    stx OAMADDR
    lda #>_shadow_OAM
    sta OAMDMA

    ; Perform initialization of DATA area
    lda #<s__XINIT
    sta ___memcpy_PARM_2
    lda #>s__XINIT
    sta ___memcpy_PARM_2+1
    lda #<l__XINIT
    sta ___memcpy_PARM_3
    lda #>l__XINIT
    sta ___memcpy_PARM_3+1
    lda #<s__DATA
    ldx #>s__DATA
    jsr ___memcpy

    ; Set bank to first
    lda #0x00
    sta *__current_bank
    ; Set palette shadow
    jsr __crt0_setPalette
    lda #VRAM_DELAY_CYCLES_X8
    sta *__vram_transfer_buffer_num_cycles_x8
    lda #0
    sta *__vram_transfer_buffer_pos_w
    ; 
    lda #(PPUMASK_SHOW_BG | PPUMASK_SHOW_SPR | PPUMASK_SHOW_BG_LC | PPUMASK_SHOW_SPR_LC)
    sta *_shadow_PPUMASK
    lda #0x80
    sta *__oam_valid_display_on
    ; enable NMI
    lda #(PPUCTRL_NMI | PPUCTRL_SPR_CHR)
    sta *_shadow_PPUCTRL
    sta PPUCTRL
    jsr .deferred_isr_reset
    ; Call main
    jsr _main
    ; main finished - loop forever
__crt0_waitForever:
    jmp __crt0_waitForever

.bndry 0x100
.do_hblank_writes:
    .define .reg_write_index    "__crt0_NMITEMP+1"
    .define .lda_PPUADDR        "__crt0_NMITEMP+2"
    .define .ldx_PPUMASK        "__crt0_NMITEMP+3"
    
    ; Delay to make hblank at end of scanline 0
    ldx #10
0$:
    dex
    bne 0$
    clc
    nop

    sty *.reg_write_index

    lda #0
    sta *.acc
1$:
    ldx __lcd_isr_delay_num_scanlines,y
    cpx #1
    beq 3$      ; Skip delay if next scanline
    cpx #0
    beq 2$      ; Exit if empty buffer (no calls were made within frame)
    dex
    jsr .delay_to_lcd_scanline
    jsr .delay_12_cycles
    jsr .delay_28_cycles
    nop
    nop
    nop
    nop
    lda *0x00
3$:

    ; Delay for 35.666 NTSC cycles / 28.5625 PAL cycles
    jsr .delay_fractional

    ; Pre-write PPUADDR (1st write) and y-scroll
    sty PPUADDR
    lda __lcd_isr_scroll_y,y
    sta PPUSCROLL
    ; A <- PPUADDR (2nd write)
    lda __lcd_isr_ppuaddr_lo,y
    sta *.lda_PPUADDR
    ldx __lcd_isr_PPUMASK,y
    stx *.ldx_PPUMASK
    ; X <- SCROLLX
    ldx __lcd_isr_scroll_x,y
    ; Y <- PPUCTRL
    lda __lcd_isr_PPUCTRL,y
    tay
    lda *.lda_PPUADDR
    ;
    ; Write 4 PPU registers in following order.
    ;
    ; 1. PPUSCROLL          (needs to be written to set fine-x)
    ; 2. PPUADDR 2nd write  (highest priority as needs to happen before the two-tile pre-fetch) 
    ; 3. PPUCTRL            (PPU pattern table switch can affect two-tile pre-fetch)
    ; 4. PPUMASK            (emphasis and render on/off are maybe less distracting?)
    ;
    ; TODO: Self-modifying code could build a non-redundant write sequence in RAM.
    ;
    stx PPUSCROLL
    sta PPUADDR
    ldx *.ldx_PPUMASK
    stx PPUMASK
    sty PPUCTRL

    inc *.reg_write_index
    ldy *.reg_write_index
    jmp 1$
2$:
    rts

; Interrupt / RESET vector table
.area VECTORS (ABS)
.org 0xfffa
.dw __crt0_NMI
.dw __crt0_RESET
.dw __crt0_IRQ
