.module SetAttributeXY

.include "global.s"

.area GBDKOVR (PAG, OVR)
_set_bkg_attribute_xy_nes16x16_PARM_3::    .ds 1
.x_odd:                                     .ds 1
.y_odd:                                     .ds 1
.val:                                       .ds 1

.area _HOME

_set_bkg_attribute_xy_nes16x16::
    lsr
    ror *.x_odd
    tay
    txa
    lsr
    ror *.y_odd
    pha
    asl
    asl
    asl
    and #0x38
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
    and _attribute_shadow,y
    ora *.val
    sta _attribute_shadow,y
    ; Set dirty bit for row.
    ; Assume writing rows, as the potential to optimize column writing is limited anyway.
    pla
    tay
    lda .bitmask_dirty_tab,y
    ora *_attribute_row_dirty
    sta *_attribute_row_dirty
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
