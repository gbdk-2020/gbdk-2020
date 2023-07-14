.include "global.s"

.area OSEG (PAG, OVR)

.area _ZP (PAG)
__lcd_scanline::    .ds 1

.area _INITIALIZED
.jmp_to_VBL_isr::   .ds 3
.jmp_to_LCD_isr::   .ds 3

.area _INITIALIZER
; .jmp_to_VBL_isr
rts
nop
nop
; .jmp_to_LCD_isr
rts
nop
nop

.area _CODE

_add_VBL::
.add_VBL::
    ldy #0
    beq .add_common
_add_LCD::
.add_LCD::
    ldy #3
.add_common:
    ; Remove old handler
    jsr .remove_common
    ; Store new handler address
    sta .jmp_to_VBL_isr+1,y
    txa
    sta .jmp_to_VBL_isr+2,y
    ; Re-enable handler by replacing RTS with JMP instruction
    lda #0x4C
    sta .jmp_to_VBL_isr,y
    rts

_remove_VBL::
.remove_VBL::
    ldy #0
    beq .remove_common
_remove_LCD::
.remove_LCD::
    ldy #3
.remove_common:
    pha
    ; Replace jump instruction with RTS instruction to disable handler
    lda #0x60
    sta .jmp_to_VBL_isr,y
    pla
.return_instruction:
    rts
