    .include    "global.s"

    .area   _HOME

    .define .lcd_scanline_previous "REGTEMP"
    .define .lcd_buf_index "REGTEMP+1"
    .define .lcd_buf_end "REGTEMP+2"

;
; Writes shadow registers to buffer
;
; Input:
;  X: Scanline number
;
.write_shadow_registers_to_buffer::
    ; Copy shadow registers
    ldy *.lcd_buf_index
    lda *_shadow_PPUMASK
    sta __lcd_isr_PPUMASK,y
    lda *_shadow_PPUCTRL
    sta __lcd_isr_PPUCTRL,y
    lda *_bkg_scroll_x
    sta __lcd_isr_scroll_x,y
    lsr
    lsr
    lsr
    sta __lcd_isr_ppuaddr_lo,y
    ; Add _bkg_scroll_y+1 to _lcd_scanline to generate final Y-scroll, with 239->0 wrap-around
    txa
    sec
    adc *_bkg_scroll_y
    bcc 1$
    sbc #.SCREENHEIGHT
1$:
    cmp #.SCREENHEIGHT
    bcc 2$
    sbc #.SCREENHEIGHT
2$:
    sta __lcd_isr_scroll_y,y
    and #0xF8
    asl
    asl
    ora __lcd_isr_ppuaddr_lo,y
    sta __lcd_isr_ppuaddr_lo,y
    rts

;
; Resets the deferred ISR.
;
; This effectively omits the double-buffering delay and writes the shadow registers to the first buffer.
; It is intended to be used when display is re-enabled after being turned off, to display with reasonable values.
;
; Note that in contrast to .run_deferred_isr_handlers, this will NOT actually run VBL / LCD handlers, so
; can still cause glitches.
;
.deferred_isr_reset::
    ; Prepare VBL buffer data (need to start at scanline -1 = SCREENHEIGHT-1 for correct Y scroll)
    lda #0
    sta *__hblank_writes_index
    sta *.lcd_buf_index
    sta __lcd_isr_delay_num_scanlines+1
    ldx #.SCREENHEIGHT-1
    jmp .write_shadow_registers_to_buffer

;
; Resets the deferred ISR, then runs VBL and LCD handlers twice to initialize buffers with valid data.
;
.deferred_isr_reset_and_init::
    jsr .deferred_isr_reset
    jsr .deferred_isr_run
    jmp .deferred_isr_run

;
; Executes the deferred VBL/LCD handlers.
;
; After each ISR handler has run, PPU shadow registers are written to a buffer 
; which is consumed by the vblank NMI handler.
; Double-buffering is used to avoid buffer locking / race conditions.
;
.deferred_isr_run::
    ; Save shadow registers that VBL or LCD isr could change
    lda *_shadow_PPUMASK
    pha
    lda *_shadow_PPUCTRL
    pha
    lda *_bkg_scroll_x
    pha
    lda *_bkg_scroll_y
    pha
    
    ; Allow VBL isr to modify shadow registers if present
    jsr .jmp_to_VBL_isr

    ; Set initial scanline value
    lda #0xFF
    sta *.lcd_scanline_previous

    lda *__hblank_writes_index
    clc
    adc #.MAX_DEFERRED_ISR_CALLS
    cmp #(2*.MAX_DEFERRED_ISR_CALLS)
    bcc 20$
    lda #0
20$:
    sta *.lcd_buf_index
    clc
    adc #.MAX_DEFERRED_ISR_CALLS
    sta *.lcd_buf_end

    ; Write shadow registers as first LCD buffer entry (actually VBL)
    ldy *.lcd_buf_index
    ldx #.SCREENHEIGHT-1
    jsr .write_shadow_registers_to_buffer
    iny
    sty *.lcd_buf_index

    ; Ensure second entry (actual LCD) starts off with zero (end-of-list)
    lda #0
    sta __lcd_isr_delay_num_scanlines,y
    ; Skip to end if LCD isr functionality is disabled (0x60 = RTS means LCD isr disabled)
    lda .jmp_to_LCD_isr
    cmp #0x60
    beq .deferred_isr_run_done

    lda *.lcd_scanline_previous

    jmp 2$
1$:
    pla
    sta *.lcd_scanline_previous
2$:
    ; We are done if next scanline is <= the previous one
    cmp #0xFF
    beq 3$
    cmp *__lcd_scanline
    bcs .deferred_isr_run_done
3$:
    ;
    ldy *.lcd_buf_index
    lda *__lcd_scanline
    ; We are done if next LCD scanline >= SCREENHEIGHT
    cmp #.SCREENHEIGHT
    bcs .deferred_isr_run_done
    pha
    sec
    sbc *.lcd_scanline_previous
    sta __lcd_isr_delay_num_scanlines,y
    ; Call LCD isr
    jsr .jmp_to_LCD_isr
    ; Grab previous LCD scanline value from stack and store in X
    pla
    tax
    pha
    jsr .write_shadow_registers_to_buffer
       
    iny
    sty *.lcd_buf_index
    cpy *.lcd_buf_end
    bne 1$
    
    ; Clear last-scanline-value from stack
    pla

.deferred_isr_run_done:
    ; Flip __hblank_writes_index for NMI handler
    ldy #0
    lda *__hblank_writes_index
    bne 21$
    ldy #.MAX_DEFERRED_ISR_CALLS
21$:
    sty *__hblank_writes_index

    ; Restore shadow registers
    pla
    sta *_bkg_scroll_y
    pla
    sta *_bkg_scroll_x
    pla
    sta *_shadow_PPUCTRL
    pla
    sta *_shadow_PPUMASK

    rts