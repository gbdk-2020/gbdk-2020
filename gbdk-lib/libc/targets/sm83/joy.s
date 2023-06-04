        .include "global.s"

        .module joypad_interrupts

        .area	_HEADER_JOY (ABS)

        .org	0x60        	; JOY
.int_JOY:
        push af
        push hl
        push bc
        push de
        jp .int_0x60

        .area   _GSINIT

        ld de, #.int_call_chain
        ld hl, #.int_0x60
        ld c, #((.INT_CALL_CHAIN_SIZE + 1) * 3)
        rst 0x30                ; memcpysmall

        .area	_HOME

_add_JOY::
.add_JOY::
        ld hl, #.int_0x60
        jp .add_int

_remove_JOY::
.remove_JOY::
        ld hl, #.int_0x60
        jp .remove_int

        .area        _DATA

.int_0x60::
        .blkb ((.INT_CALL_CHAIN_SIZE + 1) * 3)