; font.ms
;
;       Michael Hope, 1999
;       michaelh@earthling.net
;       Distrubuted under the Artistic License - see www.opensource.org
;
        .include        "global.s"

        .area	GBDKOVR (PAG, OVR)
        _temp_word:                 .ds 2

        .globl  .cr_curs
        .globl  .adv_curs
        .globl  .cury, .curx
        .globl  .display_on, .display_off

        ; Structure offsets
        sfont_handle_sizeof     = 3
        sfont_handle_font       = 1
        sfont_handle_first_tile = 0

        ; Encoding types - lower 2 bits of font
        FONT_256ENCODING        = 0
        FONT_128ENCODING        = 1
        FONT_NOENCODING         = 2

        ; Other bits
        FONT_COMPRESSED         = 4
        FONT_BCOMPRESSED        = 2
        
        .CR                     = 0x0A          ; Unix
        .SPACE                  = 0x00

        ; Maximum number of fonts
        .MAX_FONTS              = 6

        .area   _FONT_HEADER (ABS)

        .org    .MODE_TABLE+4*.T_MODE
        jmp     .tmode

        .module font.ms

        .globl  _set_bkg_1bpp_data, _set_bkg_data

        .area   _DATA
.curx::                         ; Cursor position
        .ds     0x01
.cury::
        .ds     0x01

        .area   _XINIT
        .db     0x00            ; .curx
        .db     0x00            ; .cury

        .area   _BSS
        ; The current font
font_handle_base:
font_current::
        .ds     sfont_handle_sizeof
        ; Cached copy of the first free tile
font_first_free_tile::
        .ds     1
        ; Table containing descriptors for all of the fonts
font_table::
        .ds     sfont_handle_sizeof*.MAX_FONTS

        .area   _HOME

_font_load_ibm::
        lda #<_font_ibm
        ldx #>_font_ibm
        jmp font_load
        rts

; Load the font HL
font_load::
        sta *_temp_word
        stx *_temp_word+1

        jsr .display_off

        ; numTiles
        ldy #1
        lda [*_temp_word],y
        tax

        ; Find the first free font entry
        ldy #sfont_handle_font
        ldx #.MAX_FONTS
font_load_find_slot:
        lda font_table,y        ; Check to see if this entry is free
        iny
        ora font_table,y        ; Free is 0000 for the font pointer
        beq font_load_found
        iny
        iny
        dex
        bne font_load_find_slot
        jmp font_load_exit      ; Couldn't find a free space
font_load_found:
        ; _temp_word points to the end of the free font table entry
        lda *_temp_word+1
        sta font_table,y        ; Copy across the font struct pointer
        dey
        lda *_temp_word
        sta font_table,y

        lda font_first_free_tile
        dey
        sta font_table,y
        ; Set this new font to be the default
        jsr font_set
        tya
        pha
        ; Only copy the tiles in if were in text mode
        lda .mode
        and #.T_MODE
        beq font_load_skip_copy_current
        jsr font_copy_current
font_load_skip_copy_current:
        ; Increase the 'first free tile' counter
        lda #<font_current+sfont_handle_font
        sta .tmp
        lda #>font_current+sfont_handle_font
        sta .tmp+1
        ldy #0
        lda [*.tmp],y
        iny
        tax
        lda [*.tmp],y
        sta .tmp+1
        stx .tmp
        lda font_first_free_tile
        clc
        adc [*.tmp],y   ; Number of tiles used
        sta font_first_free_tile
font_load_exit:
        ;; Turn the screen on
        jsr .display_on
        pla             ; Return font setup in A
        rts

        ; Copy the tiles from the current font into VRAM
font_copy_current::
        ; Find the current font data
        lda font_current+sfont_handle_font
        sta *.tmp
        lda font_current+sfont_handle_font+1
        sta *.tmp+1
        ldy #0
        lda [*.tmp],y
        iny
        pha
        lda [*.tmp],y
        iny
        tax
;
        pla
        pha
        and #3
        cmp #FONT_128ENCODING
        beq 1$
        cmp #FONT_NOENCODING
        beq 2$
        ; 256ENCODING - add #256 to .tmp -> inc .tmp+1
        inc .tmp+1
        jmp 2$
1$:
        ; 128ENCODING - add #128 to y
        tya
        ora #128
        tay
2$:
        tya
        clc
        adc *.tmp
        sta *_set_bkg_data_PARM_3
        sta *_set_bkg_1bpp_data_PARM_3
        lda *.tmp+1
        adc #0
        sta *_set_bkg_data_PARM_3+1
        sta *_set_bkg_1bpp_data_PARM_3+1
        pla
        and #FONT_COMPRESSED
        bne 3$
        lda font_current+sfont_handle_first_tile
        jsr _set_bkg_data
        jsr .display_on
        rts
3$:
        lda font_current+sfont_handle_first_tile
        jsr _set_bkg_1bpp_data
        jsr .display_on
        rts

        ; Set the current font to Y
font_set::
        lda font_table,y
        iny
        sta font_current
        lda font_table,y
        iny
        sta font_current+1
        lda font_table,y
        iny
        sta font_current+2
        dey
        dey
        dey
        rts

        ;; Print a character with interpretation
.put_char::
        ; See if it's a special char
        cmp #.CR
        bne 1$

; Now see if were checking special chars
        lda .mode
        and #.M_NO_INTERP
        bne 2$
        jsr .cr_curs
        rts

2$:
1$:
        jsr .set_char
        jmp .adv_curs

        ;; Print a character without interpretation
.out_char::
        jsr .set_char
        jmp .adv_curs

        ;; Delete a character
.del_char::
        jsr .rew_curs
        lda #.SPACE
        jmp .set_char

        ;; Print the character in A
.set_char:
        pha
        lda font_current+2
        ; Must be non-zero if the font system is setup (cant have a font in page zero)
        bne 3$

        ; Font system is not yet setup - init it and copy in the ibm font
        ; Kind of a compatibility mode
        jsr _font_init
        
        ; Need all of the tiles
        lda #0
        sta font_first_free_tile
        jsr _font_load_ibm
3$:
; Compute which tile maps to this character
        pla
        pha
        tax
        lda font_current+sfont_handle_font
        sta *.tmp
        lda font_current+sfont_handle_font+1
        sta *.tmp+1
        ldy #0
        lda [*.tmp],y
        and #3
        cmp #FONT_NOENCODING
        beq set_char_no_encoding
        iny
        iny
; Now at the base of the encoding table
; E is set above
        pla
        pha
        clc
        adc .identity,y
        tay
        lda [*.tmp],y       ; That's the tile!
        tax
set_char_no_encoding:
        pla
        txa
        clc
        adc font_current+sfont_handle_first_tile
        sta _set_bkg_tile_xy_PARM_3
        lda .curx
        ldx .cury
        jsr _set_bkg_tile_xy
        rts


_putchar::
        jmp .put_char

_setchar::
        jmp .set_char

_font_load::
        jmp font_load

_font_set::
        tay
        jmp font_set

_font_init::
        .globl  .tmode
        jsr .tmode
        lda #0
        sta font_first_free_tile
        ; Clear the font table
        ldy #sfont_handle_sizeof*.MAX_FONTS-1
1$:
        sta font_table,y
        dey
        bpl 1$
        lda #3
        sta .fg_colour
        lda #0
        sta .bg_colour
        jsr .cls_no_reset_pos
        rts

        
_cls::
.cls::
        lda #0
        sta .curx
        sta .cury
.cls_no_reset_pos:
        ; TODO: Make this clear entire screen to #.SPACE without manual blanking
        rts
        ; Support routines
_gotoxy::
        ; TODO: Double-check which is x vs y
        sta .curx
        stx .cury
        rts

_posx::
        lda .mode
        and #.T_MODE
        bne 1$
        jsr .tmode
1$:
        lda .curx
        rts

_posy::
        lda .mode
        and #.T_MODE
        bne 1$
        jsr .tmode
1$:
        lda .cury
        rts

        ;; Rewind the cursor
.rew_curs:
        ldy .cury
        ldx .curx
        beq 1$
        dex
        jmp 99$
1$:
        cpy #0
        beq 99$
        dey
99$:
        stx .curx
        sty .cury
        rts

.cr_curs::
        lda #0
        sta .curx
        lda #.MAXCURSPOSY
        cmp .cury
        beq 2$
        inc .cury
        jmp 99$
2$:
        jsr .scroll
99$:
        rts

.adv_curs::
        lda #.MAXCURSPOSX
        cmp .curx
        beq 1$
        inc .curx
        jmp 99$
1$:
        lda #0
        sta .curx
        lda #.MAXCURSPOSY
        cmp .cury
        beq 2$
        inc .cury
        jmp 99$
2$:
        ;; See if scrolling is disabled
        lda .mode
        and #.M_NO_SCROLL
        beq 3$
        ;; Nope - reset the cursor to (0,0)
        lda #0
        sta .cury
        sta .curx
        jmp 99$
3$:     
        jsr .scroll
99$:
        rts


        ;; Scroll the whole screen
.scroll:
        ; TODO: Read-Writes on nametable memory is not ideal on the NES for a variety of reasons.
        ; Even a performant implementation could block for around 15 frames until vblank updates have 
        ; moved all bytes.
        ; For now, just do nothing and investigate this further.
        rts


        ;; Enter text mode
.tmode::
        ;; Turn the screen off
        jsr .display_off
        jsr .tmode_out
        jsr .display_on
        rts

        ;; Text mode (out only)
.tmode_out::
        ;; Clear screen
        jsr .cls_no_reset_pos
        lda #.T_MODE
        sta .mode
        rts
