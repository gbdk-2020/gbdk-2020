        .include        "global.s"

        .title  "Font utilities"
        .module FontUtils

        .globl  .memset_small
        .globl  _font_ibm

        ; Structure offsets
        sfont_handle_sizeof     = 3
        sfont_handle_font       = 1
        sfont_handle_first_tile = 0

        ; Encoding types - lower 2 bits of font
        FONT_256ENCODING        = 0
        FONT_128ENCODING        = 1
        FONT_NOENCODING         = 2

        ; Other bits
        FONT_BCOMPRESSED        = 2
        
        ; Maximum number of fonts
        .MAX_FONTS              = 6

        .area   _BSS
        ; The current font
.start_font_vars:
.fg_colour::
        .ds     1
.bg_colour::
        .ds     1

font_current::
        .ds     sfont_handle_sizeof
        ; Cached copy of the first free tile
font_first_free_tile:
        .ds     1
        ; Table containing descriptors for all of the fonts
font_table:
        .ds     (sfont_handle_sizeof*.MAX_FONTS)
plane0:
        .ds     1
plane1:
        .ds     1
plane2:
        .ds     1
plane3:
        .ds     1
.end_font_vars:

        .area   _GSINIT

        xor a
        ld hl,#.start_font_vars
        ld c,#(.end_font_vars-.start_font_vars)
        call .memset_small
        ld a, #3
        ld (.fg_colour), a

        .area   _HOME

; void vmemcpy (unsigned int dst, const void *src, unsigned int size) __z88dk_callee __preserves_regs(iyh,iyl);
_font_init::
        xor a
        ld (font_first_free_tile), a

        ld hl, #font_table
        ld c, #(sfont_handle_sizeof*.MAX_FONTS)
        call .memset_small

        ret

_font_set::
        pop de
        pop hl
        push hl
        push de

font_set::
        ld de, #font_current
        ld bc, #sfont_handle_sizeof
        ldir
        ret

_font_color::
        pop hl
        pop de
        push de
        push hl
        ld hl, #.fg_colour
        ld (hl), e
        inc hl
        ld (hl), d
        ret

font_load_ibm::
        ld hl, #_font_ibm
        jp font_load

_font_load::
        pop de
        pop hl
        push hl
        push de

        ;; load font in HL to free slor
font_load::
        push hl
        ; find free slot
        ld b, #.MAX_FONTS
        ld hl, #(font_table+sfont_handle_font)
1$:
        ld a, (hl)
        inc hl
        or (hl)
        jr z, 2$        ; found
        inc hl
        inc hl
        dec b
        jr nz, 1$

        ld hl, #0       ; no free slot
        ret
2$:
        ; fill free slot with passed pointer
        pop de
        ld (hl), d
        dec hl
        ld (hl), e
        ld a, (font_first_free_tile)
        dec hl
        ld (hl),a          
        
        push hl
        call font_set

        call load_font_tiles

        ld hl, (font_current+sfont_handle_font)

        inc hl          ; Number of tiles used
        ld a, (font_first_free_tile)
        add (hl)
        ld (font_first_free_tile), a

        pop hl          ; return handle in hl
        ret

load_font_tiles:
        ld hl, (font_current+sfont_handle_font)

        inc hl
        ld e, (hl)
        ld d, #0
        
        dec hl
        ld a, (hl)              ; a = flags
        push af
        and #3

        ld bc, #128
        cp #FONT_128ENCODING    ; 0 for 256 char encoding table, 1 for 128 char
        jr z, 1$

        ld bc, #0
        cp #FONT_NOENCODING
        jr z, 1$

        ld bc, #256             ; Must be 256 element encoding
1$:
        inc hl
        inc hl                  ; Points to the start of the encoding table
        add hl, bc           
        ld c, l
        ld b, h                 ; BC points to the start of the tile data               

        ; Find the offset in VRAM for this font
        ld a,(font_current+sfont_handle_first_tile)        ; First tile used for this font
        ld l, a
        ld h, #0
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        ld a, #>.VDP_VRAM
        add h
        ld h, a                 ; HL = destination in VRAM

        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT
        WRITE_VDP_CMD_HL
        ld h, b
        ld l, c

        pop af                  ; Recover flags
        bit FONT_BCOMPRESSED, a ; Is this font compressed?
        jp z, 2$
        ; font_copy_compressed

        inc e
        jr 7$

8$:
        ld d, #8
9$:
        ld b, (hl)
        inc hl

        push hl
        ld c, #8
21$:
        rlc b
        ld a, (.bg_colour)
        jr nc, 20$
        ld a, (.fg_colour)
20$:
        ld hl, #plane0
        .rept 3
                rra
                rl (hl)
                inc hl
        .endm
        rra
        rl (hl)
        dec c
        jr nz, 21$
        pop hl

        ld a, (plane0)
        out (.VDP_DATA), a
        VDP_DELAY
        ld a, (plane1)
        out (.VDP_DATA), a
        VDP_DELAY
        ld a, (plane2)
        out (.VDP_DATA), a
        VDP_DELAY
        ld a, (plane3)
        out (.VDP_DATA), a

        dec d
        jr nz, 9$
7$:
        dec e
        jr  nz, 8$

        jp 6$      
2$:
        ; font_copy_uncompressed
        ld c, #.VDP_DATA
        inc e
        jr 5$
3$:
        ld b, #32
4$:        
        outi
        jr nz, 4$
5$:
        dec e
        jp  nz, 3$

6$:     
        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        ret
