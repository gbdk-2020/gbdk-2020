        .include        "global.s"

; GB-Decompress tiledata directly to VRAM
; Compatible with GBTD

        .area _CODE

_gb_decompress_vram::
        lda     hl,5(SP)
        ld      a,(hl-)
        ld      d,a
        ld      a,(hl-)
        ld      e,a
        ld      a,(hl-)
        ld      l,(hl)
        ld      h,a
        jr      gb_decompress_vram 

_gb_decompress_bkg_data::
_gb_decompress_win_data::
        ld      d, #0x90
        ldh     a, (.LCDC)
        bit     4, a
        jr      z, .load_params
_gb_decompress_sprite_data::
        ld      d, #0x80

.load_params:
        ldhl    sp, #2
        ld      a, (hl+)
        
        ; Compute dest ptr
        swap    a ; *16 (size of a tile)
        ld      e, a
        and     #0x0F ; Get high bits
        add     d ; Add base offset of target tile "block"
        ld      d, a
        ld      a, e
        and     #0xF0 ; Get low bits only
        ld      e, a

        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

; hl = source; de = dest
gb_decompress_vram::
        push    bc
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
2$:
        WAIT_STAT
        ld      a,(hl)
        ld      (de),a
        inc     de
        dec     b
        jr      nz,2$
        inc     hl
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
        WAIT_STAT
        ld      a,b     ; store word
        ld      (de),a
        inc     de
        WAIT_STAT
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
        WAIT_STAT
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
        WAIT_STAT               
        ld      a,(hl+)
        ld      (de),a
        inc     de
        dec     b
        jr      nz,8$
        jr      1$      ; next command
9$:
        pop     bc
        ret
