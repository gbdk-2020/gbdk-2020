.include "global.s"

    .area   _HOME

__switch_prg0::
    sta *__current_bank
.ifdef VRAM_MAPPER_CFG_TRANSFER
    ora *__current_vram_cfg_display_isr
.endif
    sta .identity
    rts

.ifdef VRAM_MAPPER_CFG_TRANSFER
__switch_vram_cfg_temp::
    ora *__current_vram_cfg_write
    ora *__current_bank
    sta .identity
    rts

__switch_win_addr_temp::
    pha
    lda #0
    cpx #PPUHI_WIN
    bcc 0$
    ora #MAPPER_CFG_NT_MASK
0$:
    ora *__current_vram_cfg_write
    ora *__current_bank
    sta .identity
    pla
    rts
.endif
