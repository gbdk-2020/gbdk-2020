    .include    "global.s"

    .area   _HOME

    .define .lcd_scanline_previous "REGTEMP"
    .define .lcd_buf_index "REGTEMP+1"
    .define .lcd_buf_end "REGTEMP+2"
    .define .last_scanline_value

;
; Writes shadow registers to buffer
;
; Input:
;  X: Scanline number
;
.write_shadow_registers_to_buffer::
    inx
    cpx #.SCREENHEIGHT
    bcc 6$
    txa
    sec
    sbc #.SCREENHEIGHT
    tax
6$:
    ; Copy shadow registers
    ldy *.lcd_buf_index
.ifdef NES_WINDOW_LAYER
    lda #0
    sta __lcd_isr_mapper,y
.endif
    lda *_shadow_PPUMASK
    sta __lcd_isr_PPUMASK,y
    lda *_shadow_PPUCTRL
    sta __lcd_isr_PPUCTRL,y
.ifdef NES_WINDOW_LAYER
    ; is WIN in front of BKG?
    ; Never in front if not enabled
    lda *__oam_valid_display_on
    and #WINDOW_ON_MASK
    beq 0$
    ; always in front if _win_pos_y == 0
    lda *_win_pos_y
    beq 9$
    ; in front if current_scanline >= win_pos_y
    txa
    cmp #.SCREENHEIGHT
    bcc 7$
    sbc #.SCREENHEIGHT
7$:
    cmp *_win_pos_y
    bcc 0$
9$:
    ; Enable window
    lda __lcd_isr_mapper,y
    ora #PPUHI_WIN
    sta __lcd_isr_mapper,y
    ; Write window registers
    lda #0
    sec
    sbc *_win_pos_x
    sta __lcd_isr_scroll_x,y
    lsr
    lsr
    lsr
    sta __lcd_isr_ppuaddr_lo,y
    txa
    sec
    sbc *_win_pos_y
    cmp #.SCREENHEIGHT
    bcc 8$
    sbc #.SCREENHEIGHT
8$:
    jmp 2$
.endif
0$:
    lda *_bkg_scroll_x
    sta __lcd_isr_scroll_x,y
    lsr
    lsr
    lsr
    sta __lcd_isr_ppuaddr_lo,y
    ; Add _bkg_scroll_y to current scanline to generate final Y-scroll, with 239->0 wrap-around
    txa
    clc
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
.ifdef NES_WINDOW_LAYER
    lda *_win_pos_x
    pha
    lda *_win_pos_y
    pha
    lda *__oam_valid_display_on
    pha
.endif

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

.ifdef NES_WINDOW_LAYER
    ldx *__lcd_scanline
    jsr .deferred_isr_handle_window
    pla
    txa
    pha
.endif

    sec
    sbc *.lcd_scanline_previous
    sta __lcd_isr_delay_num_scanlines,y
    ; Call LCD isr
    jsr .jmp_to_LCD_isr
    ; Grab LCD scanline value from stack and store in X
    pla
    tax
    pha
    jsr .write_shadow_registers_to_buffer

30$:
    iny
    sty *.lcd_buf_index
    cpy *.lcd_buf_end
    bne 1$
    
    ; Clear last-scanline-value from stack
    pla

.deferred_isr_run_done:

.ifdef NES_WINDOW_LAYER
    ldx #.SCREENHEIGHT
    jsr .deferred_isr_handle_window
.endif

    ; Flip __hblank_writes_index for NMI handler
    ldy #0
    lda *__hblank_writes_index
    bne 21$
    ldy #.MAX_DEFERRED_ISR_CALLS
21$:
    sty *__hblank_writes_index

.ifdef NES_WINDOW_LAYER
    pla
    sta *__oam_valid_display_on
    pla
    sta *_win_pos_y
    pla
    sta *_win_pos_x
.endif
    pla
    sta *_bkg_scroll_y
    pla
    sta *_bkg_scroll_x
    pla
    sta *_shadow_PPUCTRL
    pla
    sta *_shadow_PPUMASK

    rts

.ifdef NES_WINDOW_LAYER
;
; Injects an extra scanline for window switch
;
.deferred_isr_handle_window:
;;;
    ; If window is enabled and WY_REG is smaller than next LCD scanline, inject a deferred ISR for Window start / finish	
    lda *__oam_valid_display_on
    and #WINDOW_ON_MASK
    beq 21$
    txa
    cmp *_win_pos_y
    bcs 22$
    ; lcd_scanline_next < win_pos_y
    jmp 21$
22$:
    ; lcd_scanline_next >= win_pos_y -> win_pos_y < lcd_scanline_next
    lda *_win_pos_y
    beq 21$
    sec
    sbc *.lcd_scanline_previous
    sta __lcd_isr_delay_num_scanlines,y

    ; Make window scroll Y coordinate source data from top of nametable
    ldx *_win_pos_y
    dex
    jsr .write_shadow_registers_to_buffer

    ; Grab LCD scanline value from win_pos_y and store in X
    dex
    iny
    sty *.lcd_buf_index
    ; TODO: Check to not overflow deferred ISR buffer
21$:
    rts
.endif
