.include "global.s"
.include "mapper_macros.s"

.define .tempA "(REGTEMP+7)"

;
; Banked call support function.
;
;
; For a banked call to function "func", sdcc will generate the following sequence:
;
; jsr __sdcc_bcall  <-- Call to this support function
; .db b_func        <-- Bank of func
; .dw func-1        <-- Address of func - 1.
;
___sdcc_bcall::
    ; save potential function parameter in A
    sta *.tempA
    ; Store old return address to *.tmp, and add +3 to return address on stack (to skip parameters) 
    pla
    sta *.tmp
    clc
    adc #3
    tay
    pla
    sta *.tmp+1
    adc #0
    pha
    tya
    pha
    ; Save old bank on stack
    lda *__current_bank
    pha
    ; Perform banked call
    jsr 2$
    ; Returned from banked call
    sta *.tempA 
    pla
    sta *__current_bank
    SWITCH_PRG0_A
    ; restore potential function return value to A
    lda *.tempA
    ; Return to original bank
    rts
2$:
    ldy #0x03
    ; Read address (stored after jsr ___sdcc_bcall and bank number)
    lda [*.tmp],y
    pha
    dey
    lda [*.tmp],y
    pha
    dey
    ; Read bank number (stored after jsr ___sdcc_bcall)
    lda [*.tmp],y
    ; Switch bank
    sta *__current_bank
    SWITCH_PRG0_A
    ; restore potential function parameter to A
    lda *.tempA
    ; Jump to banked function via RTS
    rts
