        .include        "global.s"

        ;; Data
        .area   _INITIALIZED
__current_1bpp_colors::
        ;; Foreground drawing colour
.fg_colour::    
        .ds     1
        ;; Background drawing colour
.bg_colour::    
        .ds     1
        ;; Drawing mode (.SOILD etc)
.draw_mode::
        .ds     1

        .area   _INITIALIZER

        .db     0x03    ; .fg_colour
        .db     0x00    ; .bg_color
        .db     0x00    ; .draw_mode

        .area _HOME
_set_1bpp_colors_ex::
_color::                        
        LDA     HL,2(SP)        ; Skip return address and registers
        LD      A,(HL+)         ; A = Foreground
        LD      (.fg_colour),a
        LD      A,(HL+)
        LD      (.bg_colour),a
        LD      A,(HL)
        LD      (.draw_mode),a
        RET
