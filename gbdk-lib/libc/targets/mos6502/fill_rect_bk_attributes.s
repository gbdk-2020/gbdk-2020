    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    .fill_bkg_rect_attributes_padding::    .ds 5   ; Padding to avoid clash with _set_bkg_attribute_xy_nes16x16
    _fill_bkg_rect_attributes_PARM_3::
    _fill_win_rect_attributes_PARM_3::     .ds 1
    _fill_bkg_rect_attributes_PARM_4::
    _fill_win_rect_attributes_PARM_4::     .ds 1
    _fill_bkg_rect_attributes_PARM_5::
    _fill_win_rect_attributes_PARM_5::     .ds 1
    .xpos:                      .ds 1
    .ypos:                      .ds 1
    .xpos_save:                 .ds 1
    .width_save:                .ds 1

    .area   _HOME

.ifdef NES_WINDOW_LAYER
_fill_win_rect_attributes::
    tay
    lda *__current_vram_cfg_write
    pha
    ora #PPUHI_WIN
    sta *__current_vram_cfg_write
    tya
    jsr _fill_bkg_rect_attributes
    pla
    sta *__current_vram_cfg_write
    rts
.endif

_fill_bkg_rect_attributes::
    .define .width     "_fill_bkg_rect_attributes_PARM_3"
    .define .height    "_fill_bkg_rect_attributes_PARM_4"
    .define .attribute "_fill_bkg_rect_attributes_PARM_5"
    lsr
    sta *.xpos
    sta *.xpos_save
    txa
    lsr
    sta *.ypos
    ; width /= 2, round up
    lda *.width
    lsr
    adc #0
    sta *.width
    sta *.width_save
    ; height /= 2, round up
    lda *.height
    lsr
    adc #0
    sta *.height
    lda *.attribute
    sta *_set_bkg_attribute_xy_nes16x16_PARM_3
1$:
    lda *.xpos
    and #(.DEVICE_SCREEN_BUFFER_WIDTH/2)-1
    ldx *.ypos
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write    ; Note: Assumes PPUHI_WIN = 0x80
    bpl 2$
    jsr _set_win_attribute_xy_nes16x16
    jmp 3$
2$:
.endif
    jsr _set_bkg_attribute_xy_nes16x16
3$:
    inc *.xpos
    dec *.width
    bne 1$
    lda *.xpos_save
    sta *.xpos
    lda *.width_save
    sta *.width
    inc *.ypos
    lda *.ypos
    cmp #(.DEVICE_SCREEN_BUFFER_HEIGHT/2)
    bne 0$
    lda #0
0$:
    sta *.ypos
    dec *.height
    bne 1$
    rts
