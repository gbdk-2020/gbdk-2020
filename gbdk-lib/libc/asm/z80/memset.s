        .area   _CODE

;; void *memset (void *s, int c, size_t n) __z88dk_callee;
_memset::
        pop af
        pop hl
        pop de
        pop bc
        push af

        ld a, b
        or c
        ret z

        ld (hl), e
        dec bc

        ld a, b
        or c
        ret z

        ld d, h
        ld e, l
        inc de

        ldir
        ret

