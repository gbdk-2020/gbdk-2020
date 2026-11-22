;--------------------------------------------------------------------------
;  setjmp.s
;
;  Copyright (C) 2026, Phidias618
;
;--------------------------------------------------------------------------

        .module longjmp

        .area   _HOME

        .globl ___setjmp

___setjmp:
        ; save sp
        ldhl sp, #2
        ld a, l
        ld l, e
        ld e, h
        ld h, d
        
        ld (hl+), a
        ld a, e
        ld (hl+), a
        
        ; save return address
        pop de
        ld a, e
        ld (hl+), a
        ld (hl), d
        
        ; return 0
        ld l, e
        ld h, d
        ld bc, #0
        jp (hl)

        

.globl _longjmp

_longjmp:
        ; ensure that the return value is not zero
        ld a, b
        or c
        jr nz, 0$
        inc c
0$:     
        ; restore sp
        ld a, (de)
        ld l, a
        inc de
        ld a, (de)
        ld h, a
        inc de
        ld sp, hl

        ; get return address
     	ld h, d
		ld l, e
		ld a, (hl+)
		ld h, (hl)
		ld l, a
		jp (hl)
