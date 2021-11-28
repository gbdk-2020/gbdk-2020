        .module strcmp

        .area   _HOME

; int memcmp(const void *buf1, const void *buf2, size_t count)
_memcmp::
        pop hl
        pop de
        pop bc
        ex (sp), hl
        ex de, hl

        inc     d
        inc     e
        jr      3$

1$:     
        ld      a,(bc)
        sub     (hl)            ; s1[i]==s2[i]?
        jr      nz, 2$          ; -> Different
        
        inc     bc
        inc     hl
3$:
        dec     e
        jr      nz, 1$
        dec     d
        jr      nz, 1$

        ld      hl, #0
        ret

2$:
        ld      hl,#1
        ret     c

        ld      hl,#-1
        ret