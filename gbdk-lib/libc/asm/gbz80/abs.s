        .module abs

        .area   _HOME

; int abs(int)
_abs::
        lda     HL, 3(SP)   ; 3
        ld      A,(HL-)     ; 2 Load high byte.
        ld      D,A         ; 1
        ld      E,(HL)      ; 2 Load low byte
        add     A,A         ; 1 Sign check.
        ret     nc          ; 5/2 Return if positive.
        xor     A           ; 1 A=0.
        sub     E           ; 1 Calculate 0-(low byte).
        ld      E,A         ; 1
        ld      A,#0        ; 2 A=0. Can't use xor A because carry flag needs to be preserved. 
        sbc     D           ; 1 Calculate 0-(high byte)-carry.
        ld      D,A         ; 1
        ret                 ; 4
