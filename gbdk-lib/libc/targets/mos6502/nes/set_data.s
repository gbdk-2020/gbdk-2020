    .module SetData
    .include "global.s"

    .area	GBDKOVR (PAG, OVR)
    _set_sprite_data_PARM_3::
    _set_bkg_data_PARM_3::      .ds 2
    .ppu_addr_lo:               .ds 1
    .ppu_addr_hi:               .ds 1
    src:                        .ds 2
    tmpX:                       .ds 1

    .area   _HOME

_set_bkg_data::
    sta *.ppu_addr_lo
    lda *_set_bkg_data_PARM_3
    sta *src
    lda *_set_bkg_data_PARM_3+1
    sta *src+1
    lda *_shadow_PPUCTRL
    and #PPUCTRL_BG_CHR
    lsr
    jmp .copy_tiles
_set_sprite_data::
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
    ; plane0 - 8 even bytes
    ldy #0
    jsr .copy_tiles_1plane
    ; plane1 - 8 odd bytes
    ldy #1
    jsr .copy_tiles_1plane
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
    ldx *__vram_transfer_buffer_pos_w
    ldy #0
    jsr .copy_tiles_indirect_2planes
    lda .identity+16,x
    sta *__vram_transfer_buffer_pos_w
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

.copy_tiles_1plane:
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    lda [*src],y
    iny
    iny
    sta PPUDATA
    ;
    rts

.copy_tiles_indirect_2planes:
    lda [*src],y
    iny
    sta __vram_transfer_buffer+0,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+8,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+1,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+9,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+2,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+10,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+3,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+11,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+4,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+12,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+5,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+13,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+6,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+14,x
    ;
    lda [*src],y
    iny
    sta __vram_transfer_buffer+7,x
    lda [*src],y
    iny
    sta __vram_transfer_buffer+15,x
    ;
    rts
