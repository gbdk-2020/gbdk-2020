    .ez80

    .area   _CODE

    .globl _memcpy
    .globl ___memcpy

; The Z80 has the ldir instruction, but the chain of ldi is faster.
_memcpy:
___memcpy:
    ex de, hl
    pop af
    pop bc
    push af
    ld a, b
    or c
    ret z    
    ld a, c    
    and #7
    add a
    sub #16
    neg
    ld iy, #1$
    add iyl
    ld iyl, a
    adc iyh
    sub iyl
    ld iyh, a
    xor a
    jp (iy)
1$:
    .rept 8
        ldi
    .endm
    jp pe, 1$
    ret
