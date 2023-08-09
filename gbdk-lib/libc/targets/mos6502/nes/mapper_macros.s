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
    tay
    sta .identity,Y
.endm
