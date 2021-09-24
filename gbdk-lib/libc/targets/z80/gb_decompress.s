; GB-Deompress routine
; Compatible with GBTD

        .include        "global.s"

        .title  "GB Decompress"
        .module GBDecompress

        .area _CODE

; hl = source; de = dest
_gb_decompress::
        pop     hl
        pop     de
        ex      (sp), hl
        ex      de, hl
        push    bc
        push    de
1$:
        ld      a, (hl) ; load command
        inc     hl
        or      a
        jr      z, 9$   ; exit, if last byte
        bit     7, a
        jr      nz, 5$  ; string functions
        bit     6, a
        jr      nz, 3$
        ; RLE byte
        and     #63     ; calc counter
        inc     a
        ld      b, a
        ld      a, (hl)
        inc     hl
2$:
        ld      (de), a
        inc     de
        dec     b
        jr      nz, 2$
        jr      1$      ; next command
3$:                     ; RLE word
        and     #63
        inc     a
        ld      c, (hl) ; load word into bc
        inc     hl
        ld      b, (hl)
        inc     hl
        ex      de, hl
4$:
        ld      (hl), c
        inc     hl
        ld      (hl), b
        inc     hl
        dec     a
        jr      nz, 4$
        ex      de, hl
        jr      1$      ; next command
5$:
        bit     6, a
        jr      nz, 7$
        ; string repeat
        and     #63
        inc     a
        push    hl
        ld      c, (hl)
        inc     hl
        ld      b, (hl)
        ld      h, d
        ld      l, e
        add     hl, bc
        ld      c, a
        ld      b, #0
        ldir
        pop     hl
        inc     hl
        inc     hl
        jr      1$      ; next command
7$:                     ; string copy
        and     #63
        inc     a
        ld      c, a
        ld      b, #0
        ldir
        jr      1$      ; next command
9$:
        pop     hl
        ex      de, hl
        or      a       ; clear carry flag
        sbc     hl, de
        pop     bc
        ret
