        .module strcmp

        .area   _BASE

; int strcmp(const char *s1, const char *s2) 
_strcmp::
        lda     hl,2(sp)
        ld      a,(hl+)
        ld      e, a
        ld      a,(hl+)
        ld      d, a
        ld      a,(hl+)
        ld      h,(hl)
        ld      l,a
1$:     
        ld      a,(de)
        sub     (hl)            ; s1[i]==s2[i]?
        jr      nz, 2$          ; -> Different
        ;; A == 0
        cp      (hl)            ; s1[i]==s2[i]==0?
        
        inc     de
        inc     hl
        jr      nz, 1$          ; ^ Didn't reach a null character. Loop.
        
        ; Yes. return 0;
        ld      d, a            ; Since a == 0 this is faster than a 16 bit immediate load.
        ld      e, a
        ret
2$:
        ld      de,#-1
        ret     c

        ld      de,#1
        ret