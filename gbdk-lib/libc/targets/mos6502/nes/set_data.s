    .include "global.s"

    .area	OSEG (PAG, OVR)
    _set_sprite_data_PARM_3::
    _set_bkg_data_PARM_3::
    _set_tile_data_PARM_3::     .ds 2
    _set_tile_data_PARM_4::     .ds 1
    src:                        .ds 2

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
