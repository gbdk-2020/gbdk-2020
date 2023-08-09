        .include        "global.s"

        .title  "Colors"
        .module colors

        .area   _DATA

__current_1bpp_colors::
.fg_colour::
        .ds     1
.bg_colour::
        .ds     1

        .area   _XINIT
        .db     0x03    ; .fg_colour
        .db     0x00    ; .bg_color
