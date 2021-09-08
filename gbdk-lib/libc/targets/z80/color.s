        .include        "global.s"

        .title  "Colors"
        .module colors

        .area   _INITIALIZED

__current_1bpp_colors::
.bg_colour::
        .ds     1
.fg_colour::
        .ds     1

        .area   _INITIALIZER

        .db     0x00    ; .bg_color
        .db     0x03    ; .fg_colour
