.define MAPPER_REG ".identity"

;
; Set PRG bank to A register
;
.macro SWITCH_PRG0_A
.ifdef VRAM_MAPPER_CFG_TRANSFER
    ora *__current_vram_cfg_display_isr
.endif
    sta MAPPER_REG
.endm

;
; Set VRAM config to A/Y register
;
; Assumes A and Y already contain the same value.
;
.macro SWITCH_CFG_AY
    sta MAPPER_REG,y
.endm

;
; Set VRAM config to A register
;
; Assumes no bus conflicts. Must take exactly 4 cycles.
;
.macro SWITCH_CFG_A
    sta MAPPER_REG
.endm

;
; Set VRAM config to X register
;
; Assumes no bus conflicts. Must take exactly 4 cycles.
;
.macro SWITCH_CFG_X
    stx MAPPER_REG
.endm
