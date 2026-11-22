        .include        "global.s"

        .title  "console utilities"
        .module ConsoleUtils

        .globl  .curx, .cury

        .area   _HOME

_gotoxy::
        add #.SCREEN_X_OFS
        ld (.curx), a
        ld a, #.SCREEN_Y_OFS
        add l
        ld (.cury), a
        ret

_posx::
        ld a, (.curx)
        sub #.SCREEN_X_OFS
        ret

_posy::
        ld a, (.cury)
        sub #.SCREEN_Y_OFS
        ret