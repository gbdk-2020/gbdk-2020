        .include        "global.s"

        .globl  .int, .int_0x50
        .globl  .add_TIM

        .area   _HEADER_NESTED_TIM (ABS)

        .org    0x50            ; TIM
.int_TIM_nested:
        EI
        PUSH    AF
        PUSH    HL
        JP      .tim_isr_jump

        .area   _HOME

.tim_isr_jump:
        LD      HL, #.int_0x50
        JP      .int

_add_low_priority_TIM::
        PUSH    BC
        LDA     HL, 4(SP)       ; Skip return address and registers
        LD      C, (HL)
        INC     HL
        LD      B,(HL)
        CALL    .add_TIM
        POP     BC
        RET
