        .include "global.s"

        .module lcd_interrupts

        .area   _HEADER_LCD (ABS)

        .org    0x48            ; LCD
.int_LCD:
        push af
        push hl
        push bc
        push de
        jp .int_0x48

        .area   _GSINIT

        ld de, #.int_call_chain
        ld hl, #.int_0x48
        ld c, #((.INT_CALL_CHAIN_SIZE + 1) * 3)
        rst 0x30                ; memcpysmall

        .area   _HOME

_add_LCD::
.add_LCD::
        ld hl, #.int_0x48
        jp .add_int

_remove_LCD::
.remove_LCD::
        ld hl, #.int_0x48
        jp .remove_int

        .area   _DATA

.int_0x48::
        .blkb ((.INT_CALL_CHAIN_SIZE + 1) * 3)