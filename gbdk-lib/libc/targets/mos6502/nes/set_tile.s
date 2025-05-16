    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_tile_xy_PARM_3::   .ds 1   ; (shared with _set_vram_byte_PARM_2)
    .bkg_tile_ppu_addr::        .ds 2
    .ppuhi:                     .ds 1

    .area   _HOME

.ifdef NES_TILEMAP_S
.define PPUHI_MASK "#>PPU_NT0"
.else
.define PPUHI_MASK "*.ppuhi"
.endif

_get_bkg_xy_addr::
    ; XA = (PPU_NT0) | (X << 5) | A
    ; (A = x_pos, X = y_pos)
.ifne NT_2W
    tay
.endif
    and #NT_WIDTH-1
    sta *.bkg_tile_ppu_addr
.ifne NT_2W
    tya
    ; .ppuhi = (xpos >> 3) & 0b00000100
    lsr
    lsr
    lsr
    and #0b00000100
    ora #>PPU_NT0
    sta *.ppuhi
.else
.ifne NT_2H
    lda #>PPU_NT0
    sta *.ppuhi
.endif
.endif
.ifne NT_2H
    ; .ppuhi |= ((ypos / DEVICE_SCREEN_BUFFER_HEIGHT) << 3) & 0b00001000
    ldy #0
    txa
    cmp #NT_HEIGHT
    bcc 1$
    sbc #NT_HEIGHT  ; Assumes carry set by cmp
    iny
1$:
    tax
    tya
    asl
    asl
    asl
    and #0b00001000
    ora *.ppuhi
    sta *.ppuhi
.endif
    txa
    asl
    asl
    asl
    asl
    rol *.bkg_tile_ppu_addr+1
    asl
    ora *.bkg_tile_ppu_addr
    sta *.bkg_tile_ppu_addr
    lda *.bkg_tile_ppu_addr+1
    rol
    and #0x03
    ora PPUHI_MASK
    tax
    lda *.bkg_tile_ppu_addr
    rts

_set_bkg_tile_xy::
    jsr _get_bkg_xy_addr
    jmp _set_vram_byte
