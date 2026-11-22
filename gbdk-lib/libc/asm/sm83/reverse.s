;--------------------------------------------------------------------------
;  reverse.s
;
;  Copyright (C) 2026, Phidias618
;
;--------------------------------------------------------------------------

        .module reverse

        .area   _HOME

_reverse::
        ld h, d
        ld l, e

        ld b, d
        ld c, e
        
        ld a, (hl+)
        or a
        ret z                ; return if length == 0
        ld a, (hl+)
        or a
        ret z                ; return if length == 1
        
        ; determine the middle of the string
0$:
        ld a, (hl+)
        or a
        jr nz, 0$

        add hl, de
        rr h
        rr l

        ld d, h
        ld e, l

        jr c, 1$
        dec hl
1$:
        dec hl
        ; swap pairs of characters starting from the middle of the string until the 0 terminator is found
        ld a, (de)
2$:
        ld c, (hl)
        ld (hl-), a
        ld a, c
        ld (de), a
        
        inc de
        ld a, (de)
        or a
        jr nz, 2$                ; stop if 0 terminator

        ; return the address of the string in BC
        inc l
        ld c, l
        ret

        
