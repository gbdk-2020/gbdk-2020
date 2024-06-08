        .module abs

        .area   _HOME

; int abs(int)
_abs::
        ld  a, d
        ld  b, a
        ld  c, e
        add a
        ret nc
        xor a
        sub c
        ld  c, a
        ld  a, #0
        sbc b
        ld  b, a
        ret
