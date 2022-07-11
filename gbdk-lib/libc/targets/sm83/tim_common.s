        .include        "global.s"

        .globl  .int, .add_int, .remove_int

        .area   _HOME

.add_TIM::
        LD      HL,#.int_0x50
        JP      .add_int

_remove_TIM::
        PUSH    BC
        LDA     HL,4(SP)        ; Skip return address and registers
        LD      C,(HL)
        INC     HL
        LD      B,(HL)
        CALL    .remove_TIM
        POP     BC
        RET

.remove_TIM::
        LD      HL,#.int_0x50
        JP      .remove_int

        .area   _DATA

.int_0x50::
        .blkw   0x05
