    .include "global.s"

    .globl .fg_colour, .bg_colour

    .area _HOME
_font_color::
    sta	.fg_colour
    stx	.bg_colour
    rts
