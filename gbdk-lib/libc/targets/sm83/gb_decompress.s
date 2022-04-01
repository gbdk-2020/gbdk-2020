; GB-Deompress routine
; Compatible with GBTD

        .include        "global.s"

        .title  "GB Decompress"
        .module GBDecompress

        .area _CODE

_gb_decompress::
        lda     hl,5(SP)
        ld      a,(hl-)
        ld      d,a
        ld      a,(hl-)
        ld      e,a
        ld      a,(hl-)
        ld      l,(hl)
        ld      h,a

; hl = source; de = dest
gb_decompress::
        push    bc
        push    de
1$:
        ld      a,(hl+) ; load command
        or      a
        jr      z,9$    ; exit, if last byte
        bit     7,a
        jr      nz,5$   ; string functions
        bit     6,a
        jr      nz,3$
        ; RLE byte
        and     #63     ; calc counter
        inc     a
        ld      b,a
        ld      a,(hl+)
2$:
        ld      (de),a
        inc     de
        dec     b
        jr      nz,2$
        jr      1$      ; next command
3$:                     ; RLE word
        and     #63
        inc     a
        ld      b,(hl)  ; load word into bc
        inc     hl
        ld      c,(hl)
        inc     hl
4$:
        push    af
        ld      a,b     ; store word
        ld      (de),a
        inc     de
        ld      a,c
        ld      (de),a
        inc     de
        pop     af
        dec     a
        jr      nz,4$
        jr      1$      ; next command
5$:
        bit     6,a
        jr      nz,7$
        ; string repeat
        and     a,#63
        inc     a
        push    hl
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        ld      h,d
        ld      l,e
        add     hl,bc
        ld      b,a
6$:
        ld      a,(hl+)
        ld      (de),a
        inc     de
        dec     b
        jr      nz,6$
        pop     hl
        inc     hl
        inc     hl
        jr      1$      ; next command
7$:                     ; string copy
        and     #63
        inc     a
        ld      b,a
8$:                    
        ld      a,(hl+)
        ld      (de),a
        inc     de
        dec     b
        jr      nz,8$
        jr      1$      ; next command
9$:
        pop     hl
        ld      a, e
        sub     l
        ld      e, a
        ld      a, d
        sbc     h
        ld      d, a
        pop     bc
        
        ret
