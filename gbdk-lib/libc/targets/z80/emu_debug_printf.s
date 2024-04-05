        .include "global.s"

        .title  "EMU_debug"
        .module EMU_debug

        .area   _HOME

;;void EMU_printf(const unsigned char * fmt, ...)
_EMU_printf::
        ld hl, #2
        add hl, sp
        ld e, (hl)
        inc hl
        ld d, (hl)
        inc hl
        ex de, hl

        ld  d, d
        jr 1$
        .dw 0x6464
        .dw 0x0200
1$:
        ret

;;void EMU_fmtbuf(const unsigned char * fmt, void * data)
_EMU_fmtbuf::
        ld  d, d
        jr 1$
        .dw 0x6464
        .dw 0x0200
1$:
        ret
