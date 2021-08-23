        .include        "global.s"

        .title  "console utilities"
        .module ConsoleUtils

        .globl  .curx, .cury

        .area   _HOME

_gotoxy::
        pop hl
        pop de
        push de
        push hl

        ld a, #.SCREEN_X_OFS
        add e
        ld (.curx), a
        ld a, #.SCREEN_Y_OFS
        add d
        ld (.cury), a
        ret

_posx::
        ld a, (.curx)
        sub #.SCREEN_X_OFS
        ld l, a
        ret

_posy::
        ld a, (.cury)
        sub #.SCREEN_Y_OFS
        ld l, a
        ret