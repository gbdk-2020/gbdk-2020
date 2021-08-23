        .include        "global.s"

        .title  "screen modes"
        .module Modes

        .area   _HOME

_mode::
        pop de
        pop hl
        push hl
        push de

.set_mode::
        ld a, l
        ld (.mode), a
        ret

_get_mode::
        ld hl, #.mode
        ld l, (hl)
        ret