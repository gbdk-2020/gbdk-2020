    .include "global.s"

    .area	OSEG (PAG, OVR)
    _set_sprite_data_PARM_3::
    _set_bkg_data_PARM_3::
    _set_tile_data_PARM_3::     .ds 2
    _set_tile_data_PARM_4::     .ds 1
    src:                        .ds 2
    tmpX:                       .ds 1

    .area   _HOME

_set_tile_data::
    sta *.tmp
    lda *_set_tile_data_PARM_3
    sta *src
    lda *_set_tile_data_PARM_3+1
    sta *src+1
    ; *= 16
    lda #0
    asl *.tmp
    rol
    asl *.tmp
    rol
    asl *.tmp
    rol
    asl *.tmp
    rol
    clc
    adc *_set_tile_data_PARM_4
    sta PPUADDR
    lda *.tmp
    sta PPUADDR
    jmp .copy_tiles_loop
_set_bkg_data::
    sta *.tmp
    lda *_set_bkg_data_PARM_3
    sta *src
    lda *_set_bkg_data_PARM_3+1
    sta *src+1
    lda *_shadow_PPUCTRL
    and #PPUCTRL_BG_CHR
    lsr
    jmp .copy_tiles
_set_sprite_data::
    sta *.tmp
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
    asl *.tmp
    rol
    asl *.tmp
    rol
    asl *.tmp
    rol
    asl *.tmp
    rol
    bit *.crt0_forced_blanking
    bpl .copy_tiles_indirect
    ; Direct write
    sta PPUADDR
    lda *.tmp
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
    sta *.tmp+1
.copy_tiles_indirect_loop:
    stx *tmpX
    ;
    ldx *.tmp+1
    lda *.tmp
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
    lda *.tmp
    clc
    adc #16
    sta *.tmp
    lda *.tmp+1
    adc #0
    sta *.tmp+1
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
