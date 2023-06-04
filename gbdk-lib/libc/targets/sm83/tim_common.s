        .include "global.s"

        .module timer

        .area   _GSINIT

        ld de, #.int_call_chain
        ld hl, #.int_0x50
        ld c, #((.INT_CALL_CHAIN_SIZE + 1) * 3)
        rst 0x30                ; memcpysmall

        .area   _HOME

.add_TIM::
        ld hl, #.int_0x50
        jp .add_int

_remove_TIM::
.remove_TIM::
        ld hl, #.int_0x50
        jp .remove_int

        .area   _DATA

.int_0x50::
        .blkb ((.INT_CALL_CHAIN_SIZE + 1) * 3)
