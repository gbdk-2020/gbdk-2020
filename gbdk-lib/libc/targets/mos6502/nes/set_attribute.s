.module SetAttributeXY

.include "global.s"

.area GBDKOVR (PAG, OVR)
_set_bkg_attribute_xy_nes16x16_PARM_3::     
_set_win_attribute_xy_nes16x16_PARM_3::     .ds 1
.x_odd:                                     .ds 1
.y_odd:                                     .ds 1
.val:                                       .ds 1
.ifdef NES_WINDOW_LAYER
.winbit:                                    .ds 1
.endif

.area _HOME

.macro WRITE_ATTRIBUTE ?lbl, ?lbl2
.ifdef NES_WINDOW_LAYER
    bit *.winbit
    bpl lbl
    and _attribute_shadow_win,y
    ora *.val
    sta _attribute_shadow_win,y
    jmp lbl2
.endif
lbl:
    and _attribute_shadow,y
    ora *.val
    sta _attribute_shadow,y
lbl2:
.endm

.ifdef NES_WINDOW_LAYER
_set_win_attribute_xy_nes16x16::
    sec
    ror *.winbit
    jmp _set_attribute_xy_nes16x16_impl
.endif

_set_bkg_attribute_xy_nes16x16::
.ifdef NES_WINDOW_LAYER
    clc
    ror *.winbit
.endif
_set_attribute_xy_nes16x16_impl::
    lsr
    ror *.x_odd
    tay
    txa
.ifne NT_2H
    cmp #(NT_HEIGHT/2)
    bcc 0$
    sbc #(NT_HEIGHT/2)   ; Assumes carry set by cmp
    ora #2*AT_HEIGHT
0$:
.endif
    lsr
    ror *.y_odd
    pha
    asl
    asl
    asl
.ifne NT_2W
    asl
.endif
    and #(AT_SHADOW_WIDTH*AT_SHADOW_HEIGHT-1)
    ora .identity,y
    tay
    lda *_set_bkg_attribute_xy_nes16x16_PARM_3
    bit *.y_odd
    bpl 1$
    asl
    asl
    asl
    asl
1$:
    bit *.x_odd
    bpl 2$
    asl
    asl
2$:
    sta *.val
    lda #0
    asl *.y_odd
    rol 
    asl *.x_odd
    rol
    tax
    lda .mask_tab,x
    WRITE_ATTRIBUTE
    ; Set dirty bit for row.
    ; Assume writing rows, as the potential to optimize column writing is limited anyway.
    pla
.ifne NT_2H
    and #AT_HEIGHT-1
.endif
    tax
    lda .bitmask_dirty_tab,x
    ; Merge A with current attribute_row_dirty flag
.ifdef NES_TILEMAP_S
.ifdef NES_WINDOW_LAYER
    bit *.winbit
    bpl 9$
    ora *_attribute_row_dirty_win
    sta *_attribute_row_dirty_win
    jmp 8$
9$:
    ora *_attribute_row_dirty
    sta *_attribute_row_dirty
8$:
.endif
.endif
.ifdef NES_TILEMAP_H
    pha
    ldx #0
.ifdef NES_WINDOW_LAYER
    ; 2 nametables - window dirty byte is += 2 bytes
    bit *.winbit
    bpl 9$
    inx
    inx
9$:
.endif
    tya
    and #AT_WIDTH
    beq 10$
    inx
10$:
    pla
    ora *_attribute_row_dirty,x
    sta *_attribute_row_dirty,x
.endif
.ifdef NES_TILEMAP_V
    pha
    ldx #0
.ifdef NES_WINDOW_LAYER
    ; 2 nametables - window dirty byte is += 2 bytes
    bit *.winbit
    bpl 9$
    inx
    inx
9$:
.endif
    tya
    and #(AT_HEIGHT*AT_SHADOW_WIDTH)
    beq 10$
    inx
10$:
    pla
    ora *_attribute_row_dirty,x
    sta *_attribute_row_dirty,x
.endif
.ifdef NES_TILEMAP_F
    pha
    ldx #0
.ifdef NES_WINDOW_LAYER
    ; With 4 nametables, window dirty byte is += 4 bytes
    bit *.winbit
    bpl 9$
    inx
    inx
    inx
    inx
9$:
.endif
    tya
    and #AT_WIDTH
    beq 10$
    inx
10$:
    cpy #(AT_HEIGHT*AT_SHADOW_WIDTH)
    bcc 11$
    inx
    inx
11$:
    pla
    ora *_attribute_row_dirty,x
    sta *_attribute_row_dirty,x
.endif
    rts

.mask_tab:
.db 0b11111100
.db 0b11110011
.db 0b11001111
.db 0b00111111

.bitmask_dirty_tab:
.db 0b00000001
.db 0b00000010
.db 0b00000100
.db 0b00001000
.db 0b00010000
.db 0b00100000
.db 0b01000000
.db 0b10000000
