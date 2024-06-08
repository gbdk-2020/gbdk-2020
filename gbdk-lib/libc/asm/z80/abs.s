        .area   _CODE

        .globl _abs

_abs:
        xor a
        ex  de, hl
        ld h, a
        ld l, a
        sbc hl, de
        ex de, hl
        ret p
        ex de, hl
        ret

