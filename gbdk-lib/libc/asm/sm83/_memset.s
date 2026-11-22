        .module memset

        .area   _HOME

; void *memset (void *s, int c, size_t n)
_memset::
        ld a, c

        pop hl                ; pop return address
        pop bc                ; pop n
        push hl               ; push return address
        
        ld h, d
        ld l, e
        
        srl     b
        rr      c
        jr      nc,0$
        ld      (hl+),a
0$:     
        srl     b
        inc     b
        rr      c
        
        jr      c,2$
        jr      z,3$
1$:     
        dec     c
        ld      (hl+),a
        ld      (hl+),a
2$:
        ld      (hl+),a
        ld      (hl+),a
        jr      nz,1$
3$:
        dec     b
        jr      nz,1$

        ; return s in bc
        ld b, d
        ld c, e
        
        ret
