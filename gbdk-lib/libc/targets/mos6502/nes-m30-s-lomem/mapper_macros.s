;
; Set PRG bank to A register
;
; Trashes Y register.
;
.macro SWITCH_PRG0_A
    tay
    sta .identity,Y
.endm
