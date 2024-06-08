    .include    "global.s"

    .area   GBDKOVR (PAG, OVR)
    _set_bkg_tile_xy_PARM_3::   .ds 1   ; (shared with _set_vram_byte_PARM_2)
    .bkg_tile_ppu_addr::        .ds 2

    .area   _HOME

_get_bkg_xy_addr::
    ; XA = (PPU_NT0) | (X << 5) | A
    ; (A = x_pos, X = y_pos)
    sta *.bkg_tile_ppu_addr
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
    ora #(PPU_NT0 >> 8)
    tax
    lda *.bkg_tile_ppu_addr
    rts

_set_bkg_tile_xy::
    jsr _get_bkg_xy_addr
    jmp _set_vram_byte
