        .include        "global.s"

        .globl  .int, .int_0x50
        .globl  .add_TIM

        .area   _HEADER_TIM (ABS)

        .org    0x50            ; TIM
.int_TIM:
        PUSH    AF
        PUSH    HL
        LD      HL,#.int_0x50
        JP      .int

        .area   _HOME

_add_TIM::
        PUSH    BC
        LDA     HL,4(SP)        ; Skip return address and registers
        LD      C,(HL)
        INC     HL
        LD      B,(HL)
        CALL    .add_TIM
        POP     BC
        RET
