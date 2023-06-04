        .include "global.s"

        .module sio_interrupts

        .area    _HEADER_SIO (ABS)

        .org    0x58        ; SIO
.int_SIO:
        push af
        push hl
        push bc
        push de
        jp .int_0x58

        .area    _HOME

_add_SIO::
.add_SIO::
        ld hl, #.int_0x58
        jp .add_int

_remove_SIO::
.remove_SIO::
        ld hl, #.int_0x58
        jp .remove_int
