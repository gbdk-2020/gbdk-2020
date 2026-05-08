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
    push de

    ld a, c
    or b
    ret z

    ld a, c
    add a
    neg
    and #31
    
    add #<1$
    ld iyl, a
    adc #>1$
    sub iyl
    ld iyh, a
    xor a
    jp (iy)
1$:
    .rept 16
        ldi
    .endm
    jp pe, 1$
    pop de
    ret
