    .include "global.s"

    .globl __current_1bpp_colors

    .area	GBDKOVR (PAG, OVR)
    _set_bkg_1bpp_data_PARM_3:: .ds 2
    src:                        .ds 2
    num_tiles:                  .ds 1
    num_planes:                 .ds 1
    fg_bit:                     .ds 1
    bg_bit:                     .ds 1
    fg_bit_p0:                  .ds 1
    bg_bit_p0:                  .ds 1
    fg_bit_p1:                  .ds 1
    bg_bit_p1:                  .ds 1
    bits:                       .ds 1

    .area   _HOME

_set_bkg_1bpp_data::
    stx *num_tiles
    ;
    sta *src
    lda #0
    asl *src
    rol
    asl *src
    rol
    asl *src
    rol
    asl *src
    rol
    sta *src+1
    ;
    lda *_shadow_PPUCTRL
    and #0x10
    ora *src+1
    sta PPUADDR
    lda *src
    sta PPUADDR
    ;
    lda *_set_bkg_1bpp_data_PARM_3
    sta *src
    lda *_set_bkg_1bpp_data_PARM_3+1
    sta *src+1
    ; .fg_color[0] -> fg_bit_p0[7]
    ; .fg_color[1] -> fg_bit_p1[7]
    lda .fg_colour
    lsr
    ror *fg_bit_p0
    lsr
    ror *fg_bit_p1
    ; .bg_color[0] -> bg_bit_p0[7]
    ; .bg_color[1] -> bg_bit_p1[7]
    lda .bg_colour
    lsr
    ror *bg_bit_p0
    lsr
    ror *bg_bit_p1
    ;
_set_bkg_1bpp_data_tile_loop:
    lda #2
    sta *num_planes
    lda *fg_bit_p0
    sta *fg_bit
    lda *bg_bit_p0
    sta *bg_bit
_set_bkg_1bpp_data_plane_loop:
    ldy #0
_set_bkg_1bpp_data_byte_loop:
    lda [*src],y
    ldx #8
_set_bkg_1bpp_data_bit_loop:
    asl
    bcs _set_bkg_1bpp_data_fg
    bit *bg_bit
    bpl _set_bkg_1bpp_data_bit_loop_skip
    sec
    jmp _set_bkg_1bpp_data_bit_loop_skip
_set_bkg_1bpp_data_fg:
    bit *fg_bit
    bmi _set_bkg_1bpp_data_bit_loop_skip
    clc
_set_bkg_1bpp_data_bit_loop_skip:
    rol *bits
    dex
    bne _set_bkg_1bpp_data_bit_loop
    lda *bits
    sta PPUDATA
    iny
    cpy #8
    bne _set_bkg_1bpp_data_byte_loop
    ;
    lda *fg_bit_p1
    sta *fg_bit
    lda *bg_bit_p1
    sta *bg_bit
    dec *num_planes
    bne _set_bkg_1bpp_data_plane_loop
    ; src += 8
    lda *src
    clc
    adc #8
    sta *src
    lda *src+1
    adc #0
    sta *src+1
    dec *num_tiles
    bne _set_bkg_1bpp_data_tile_loop
    rts
