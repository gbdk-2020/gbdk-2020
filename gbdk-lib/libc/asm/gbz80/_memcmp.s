        .module strcmp

        .area   _HOME

; int memcmp(const void *buf1, const void *buf2, size_t count)
_memcmp::
        lda     hl,7(sp)
        ld      a,(hl-)
        ld      b, a
        ld      a,(hl-)
        ld      c, a
        ld      a,(hl-)
        ld      d, a
        ld      a,(hl-)
        ld      e, a
        ld      a,(hl-)
        ld      l,(hl)
        ld      h,a

        inc     b
        inc     c
        jr      3$

1$:     
        ld      a,(de)
        sub     (hl)            ; s1[i]==s2[i]?
        jr      nz, 2$          ; -> Different
        
        inc     de
        inc     hl
3$:
        dec     c
        jr      nz, 1$
        dec     b
        jr      nz, 1$

        ld      de, #0
        ret

2$:
        ld      de,#1
        ret     c

        ld      de,#-1
        ret