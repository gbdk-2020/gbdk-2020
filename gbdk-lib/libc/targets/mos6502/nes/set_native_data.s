    .module SetNativeData

    .include "global.s"

    .area	GBDKOVR (PAG, OVR)
    _set_sprite_native_data_PARM_3::
    _set_bkg_native_data_PARM_3::       .ds 2
    .ppu_addr_lo:                       .ds 1
    .ppu_addr_hi:                       .ds 1
    src:                                .ds 2
    tmpX:                               .ds 1

    .area   _HOME

_set_bkg_native_data::
    sta *.ppu_addr_lo
    lda *_set_bkg_data_PARM_3
    sta *src
    lda *_set_bkg_data_PARM_3+1
    sta *src+1
    lda *_shadow_PPUCTRL
    and #PPUCTRL_BG_CHR
    lsr
    jmp .copy_tiles
_set_sprite_native_data::
    sta *.ppu_addr_lo
    lda *_set_sprite_data_PARM_3
    sta *src
    lda *_set_sprite_data_PARM_3+1
    sta *src+1
    lda *_shadow_PPUCTRL
    and #PPUCTRL_SPR_CHR
.copy_tiles:
    lsr
    lsr
    lsr
    ; *= 16
    asl *.ppu_addr_lo
    rol
    asl *.ppu_addr_lo
    rol
    asl *.ppu_addr_lo
    rol
    asl *.ppu_addr_lo
    rol
    bit *.crt0_forced_blanking
    bpl .copy_tiles_indirect
    ; Direct write
    sta PPUADDR
    lda *.ppu_addr_lo
    sta PPUADDR
.copy_tiles_loop:
    ldy #0
1$:
    lda [*src],y
    sta PPUDATA
    iny
    cpy #16
    bne 1$
    ; src += 16
    lda *src
    clc
    adc #16
    sta *src
    lda *src+1
    adc #0
    sta *src+1
    dex
    bne .copy_tiles_loop
    rts

.copy_tiles_indirect:
    sta *.ppu_addr_hi
.copy_tiles_indirect_loop:
    stx *tmpX
    ;
    ldx *.ppu_addr_hi
    lda *.ppu_addr_lo
    jsr .ppu_stripe_begin_horizontal
    ;
    jsr .copy_tiles_indirect_2planes
    jsr .ppu_stripe_end
    ; src += 16
    lda *src
    clc
    adc #16
    sta *src
    lda *src+1
    adc #0
    sta *src+1
    ; tmp += 16
    lda *.ppu_addr_lo
    clc
    adc #16
    sta *.ppu_addr_lo
    lda *.ppu_addr_hi
    adc #0
    sta *.ppu_addr_hi
    ;
    ldx *tmpX
    dex
    bne .copy_tiles_indirect_loop
    rts

.copy_tiles_indirect_2planes:
    lda *__vram_transfer_buffer_pos_w
    clc
    adc #16
    sta *__vram_transfer_buffer_pos_w
    tax
    dex
    ldy #15
1$:
    lda [*src],y
    sta __vram_transfer_buffer,x
    dex
    dey
    bpl 1$
    rts
