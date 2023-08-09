        .include        "global.s"

        .globl  .int, .add_int, .remove_int

        .area   _HOME

.add_TIM::
        LD      HL,#.int_0x50
        JP      .add_int

_remove_TIM::
.remove_TIM::
        LD      HL,#.int_0x50
        JP      .remove_int

        .area   _DATA

.int_0x50::
        .blkw   0x05
