;
; Define for mapper register
;
.define MAPPER_WRITE_REG ".identity"

;
; Set PRG bank to A register
;
; Trashes Y register.
;
.macro SWITCH_PRG0_A
    tay
    sta .identity,Y
.endm

;
; Set PRG bank to Y register
;
; Trashes A register.
;
.macro SWITCH_PRG0_Y
    tya
    sta .identity,Y
.endm

;
; Modify PPU high address bits in A based on VRAM switch config
;
; 
;
.macro SET_PPUHI_ADDRESS_A ?.skip
.ifdef NES_WINDOW_LAYER
    bit *__current_vram_cfg_write
    bpl .skip
    ora #PPUHI_WIN
.skip:
.endif
.endm
