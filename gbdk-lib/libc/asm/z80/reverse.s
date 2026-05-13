;--------------------------------------------------------------------------
;  reverse.s
;
;  Copyright (C) 2026, Phidias618
;
;--------------------------------------------------------------------------

        .module reverse

        .area   _CODE

_reverse::
        ld d, h
        ld e, l

        xor a
        cpi
        ret z      ; return if length == 0
        cpi
        ret z      ; return if length == 1 

        ; determine the middle of the string
        ld b, a
        ld c, a
        cpir
        add hl, de
        rr h
        rr l
        
        ld b, h
        ld c, l

        jr c, 0$
        dec hl
0$:
        ; swap pairs of characters starting from the middle of the string until the 0 terminator is found
        ld a, (bc)
1$:
        dec hl
        ld e, (hl)
        ld (hl), a
        ld a, e
        ld (bc), a

        inc bc
        ld a, (bc)
        or a
        jp nz, 1$        ; exit if 0 terminator

        ; return the address of the string in de
        ex de, hl
        ret
