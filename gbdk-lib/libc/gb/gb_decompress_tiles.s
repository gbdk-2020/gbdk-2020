        .include        "global.s"

; GB-Decompress tiledata directly to VRAM
; Compatible with GBTD

.macro WRAP_VRAM regH, ?loc
        bit     3, regH
        jr      z, loc
        res     4, regH
loc:
.endm

        .area _CODE

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
        swap    a       ; *16 (size of a tile)
        ld      e, a
        and     #0x0F   ; Get high bits
        add     d       ; Add base offset of target tile "block"
        ld      d, a
        ld      a, e
        and     #0xF0   ; Get low bits only
        ld      e, a
        WRAP_VRAM d

        ld      a, (hl+)
        ld      h, (hl)
        ld      l, a

; hl = source; de = dest
gb_decompress_vram::
        push    bc
1$:
        ld      a,(hl+) ; load command
        or      a
        jp      z,9$    ; exit, if last byte
        bit     7,a
        jr      nz,5$   ; string functions
        bit     6,a
        jr      nz,3$
        ; RLE byte
        and     #63     ; calc counter
        inc     a
        ld      c,a
        ld      a,(hl+)
        ld      b,a
2$:
        WAIT_STAT
        ld      a,b
        ld      (de),a
        inc     de
        WRAP_VRAM d
        dec     c
        jr      nz,2$
        jr      1$      ; next command
        
3$:                     ; RLE word
        and     #63
        inc     a
        ld      c, a
        ld      a,(hl+)
        ld      b, a
4$:
        WAIT_STAT
        ld      a,b     ; store word
        ld      (de),a
        inc     de
        WRAP_VRAM d
        WAIT_STAT
        ld      a,(hl)
        ld      (de),a
        inc     de
        WRAP_VRAM d
        dec     c
        jr      nz,4$
        inc     hl      
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
        ld      c,a

        call    10$
        
        pop     hl
        inc     hl
        inc     hl
        jr      1$      ; next command
7$:                     ; string copy
        and     #63
        inc     a
        ld      c,a
        
        call    10$
        
        jr      1$      ; next command
9$:
        pop     bc
        ret

10$:
        WAIT_STAT               
        ld      a,(hl+)
        ld      (de),a
        inc     de
        WRAP_VRAM d
        dec     c
        jr      nz,10$
        ret
