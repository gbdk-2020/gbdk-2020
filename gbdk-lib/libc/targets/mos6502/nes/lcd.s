.include "global.s"

.area GBDKOVR (PAG, OVR)

.area _ZP (PAG)
__lcd_scanline::    .ds 1

.area   _DATA
.jmp_to_xyz_isr::
.jmp_to_TIM_isr::   .ds 3
.jmp_to_LCD_isr::   .ds 3
.jmp_to_VBL_isr::   .ds 3

.area   _XINIT
; .jmp_to_TIM_isr - default to OFF
.db 0x60
.dw .rts_instruction
; .jmp_to_LCD_isr - default to OFF
.db 0x60
.dw .rts_instruction
; .jmp_to_VBL_isr - default to ON
jmp .rts_instruction

.area _CODE
_add_TIM::
.add_TIM::
    ldy #0
    beq .add_common
_add_LCD::
.add_LCD::
    ldy #3
    bne .add_common
_add_VBL::
.add_VBL::
    ldy #6
.add_common:
    pha
    stx *.tmp
    ; Remove old handler
    jsr .irq_call_disable
    ; Store new handler address
    pla
    sta .jmp_to_xyz_isr+1,y
    lda *.tmp
    sta .jmp_to_xyz_isr+2,y
    jsr .irq_call_restore
.rts_instruction:
    rts

_remove_TIM::
.remove_TIM::
    ldy #0
    beq .remove_common
_remove_LCD::
.remove_LCD::
    ldy #3
    bne .remove_common
_remove_VBL::
.remove_VBL::
    ldy #6
.remove_common:
    ; Replace handler address with dummy address pointing to RTS instruction
    jsr .irq_call_disable
    lda .rts_instruction
    sta .jmp_to_xyz_isr+1,y
    lda .rts_instruction+1
    sta .jmp_to_xyz_isr+2,y
    jsr .irq_call_restore
.return_instruction:
    rts

.irq_call_disable:
    ; Temporarily replace JMP instruction with RTS
    ldx .jmp_to_xyz_isr,y
    lda #0x60
    sta .jmp_to_xyz_isr,y
    rts

.irq_call_restore:
    ; Rewrite old instruction (unknown - could be JMP or RTS)
    txa
    sta .jmp_to_xyz_isr,y
    rts

_set_interrupts::
    ; TIM isr
    lsr
    ldx #0x60
    bcc 0$
    ldx #0x4C
0$:
    stx .jmp_to_VBL_isr
    ; LCD isr
    lsr
    ldx #0x60
    bcc 1$
    ldx #0x4C
1$:
    stx .jmp_to_LCD_isr
    ; VBL isr
    lsr
    ldx #0x60
    bcc 2$
    ldx #0x4C
2$:
    stx .jmp_to_TIM_isr
    rts
