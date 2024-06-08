        .include "global.s"

        .title  "EMU_debug"
        .module EMU_debug

        .area   _HOME

;;void EMU_printf(const unsigned char * fmt, ...)
_EMU_printf::
        ldhl sp, #2
        ld d, h
        ld e, l
        inc de
        inc de
        ld a, (hl+)
        ld h, (hl)
        ld l, a

        ld  d, d
        jr 1$
        .dw 0x6464
        .dw 0x0200
1$:
        ret

;;void EMU_fmtbuf(const unsigned char * fmt, void * data)
_EMU_fmtbuf::
        ld h, d
        ld l, e
        ld d, b
        ld e, c

        ld  d, d
        jr 1$
        .dw 0x6464
        .dw 0x0200
1$:
        ret
