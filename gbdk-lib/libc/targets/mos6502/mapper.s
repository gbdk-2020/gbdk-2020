.include "global.s"
.include "mapper_macros.s"

__switch_prg0::
    sta *__current_bank
.ifdef VRAM_MAPPER_CFG_TRANSFER
    ora *__current_vram_cfg_write
.endif
    SWITCH_PRG0_A
    rts

.ifdef VRAM_MAPPER_CFG_TRANSFER
__switch_win_addr_temp::
    pha
    lda #0
    cpx #PPUHI_WIN
    bcc 0$
    ora #MAPPER_CFG_NT_MASK
0$:
    ora *__current_vram_cfg_write
    ora *__current_bank
    sta MAPPER_WRITE_REG
    pla
    rts
.endif

__show_window::
.ifdef NES_WINDOW_LAYER
    lda *__oam_valid_display_on
    ora #WINDOW_ON_MASK
    sta *__oam_valid_display_on
.endif
    rts

__hide_window::
.ifdef NES_WINDOW_LAYER
    lda *__oam_valid_display_on
    and #~WINDOW_ON_MASK
    sta *__oam_valid_display_on
.endif
    rts
