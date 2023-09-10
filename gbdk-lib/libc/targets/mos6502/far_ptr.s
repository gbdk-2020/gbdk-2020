    .module far_ptr

    .include "global.s"
    .include "mapper_macros.s"   

    .area   OSEG (PAG, OVR)
    _to_far_ptr_PARM_2::    .ds 2
    
    .area _ZP
    ___call_banked_ptr::
    ___call_banked_addr::   .ds 2
    ___call_banked_bank::   .ds 2
    
    .area _HOME

_to_far_ptr::
    ; XA is input 16-bit offset
    ; XA is also 16-bit return value -> nothing to move for lower 16 bits
    ; Move bank number
    ldy *_to_far_ptr_PARM_2
    sty *___SDCC_m6502_ret2
    ; Upper byte always zero - but could be used by calling function expecting int32
    ldy #0x00
    sty *___SDCC_m6502_ret3
    rts

___call__banked::
    ; save potential function parameter in A
    sta *.tmp
    ; Save old bank on stack
    lda *__current_bank
    pha
    ; Set new bank
    lda *___call_banked_bank
    sta *__current_bank
    SWITCH_PRG0_A
    ; restore potential function parameter to A
    lda *.tmp
    ; Perform banked call
    jsr 1$
    ; Returned from banked call
    ; save potential function return value in A
    sta *.tmp
    ; pull old bank and switch back
    pla
    sta *__current_bank
    SWITCH_PRG0_A
    ; restore potential function return value to A
    lda *.tmp
    rts
1$:
    jmp [*___call_banked_addr]
