        .include        "global.s"

        .title  "putchar"
        .module putchar

        .globl  font_current, font_set, font_load_ibm
        .globl  .scroll_viewport

        ; Structure offsets
        sfont_handle_sizeof     = 3
        sfont_handle_font       = 1
        sfont_handle_first_tile = 0

        ; Encoding types - lower 2 bits of font
        FONT_256ENCODING        = 0
        FONT_128ENCODING        = 1
        FONT_NOENCODING         = 2

        .area   _INITIALIZED
        ; The current font
.curx::
        .ds     1
.cury::
        .ds     1

        .area   _INITIALIZER
        .db     #.SCREEN_X_OFS  ; .curx
        .db     #.SCREEN_Y_OFS  ; .cury

        .area   _HOME

_setchar::
_putchar::
        pop hl
        pop de
        push de
        push hl

	ld a, e
        cp #.CR
        jr nz, 0$

        ld a, #.SCREEN_X_OFS
        ld (.curx), a
        jp 2$
0$:
        ld hl, (font_current+sfont_handle_font)
        ld a, h
        or l
        jr nz, 6$

        push de
        call font_load_ibm
        ld a, h
        or l
        ret z
        call font_set
        pop de
        ld hl, (font_current+sfont_handle_font)
6$:
        ld a, (hl)
        and #3
        cp #FONT_NOENCODING
        jr z, 4$
        inc hl
        inc hl
        ld d, #0
        add hl, de
        ld e, (hl)
4$:
        ld a, (font_current)
        add a, e
        ld e, a

        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT

        ld a, (.cury)
        ld l, a
        ld h, #0
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        ld a, (.curx)
        add a
        add l
        ld l, a
        adc h
        sub l
        ld h, a

        ld a, (_shadow_VDP_R2)
        rlca
        rlca
        and #0b01111000
        ld b, a
        ld c, #0
        add hl, bc

        WRITE_VDP_CMD_HL

        ld a, e
        out (.VDP_DATA), a
        VDP_DELAY
        xor a
        out (.VDP_DATA), a

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

        ld a, (.curx)
        inc a
        cp #(.SCREEN_X_OFS + .SCREEN_WIDTH)
        jr c, 5$
        ld a, #.SCREEN_X_OFS
5$:
        ld (.curx), a
        ret nz
2$:
        ld a, (.cury)
        inc a
        cp #(.SCREEN_Y_OFS + .SCREEN_HEIGHT)
        jr c, 3$
        ld a, #(.SCREEN_Y_OFS + .SCREEN_HEIGHT - 1)
        ld (.cury), a

        call .scroll_viewport
        ret
3$:
        ld (.cury), a
        ret
