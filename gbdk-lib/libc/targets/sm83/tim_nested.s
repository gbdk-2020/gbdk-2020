        .include        "global.s"

        .globl  .int, .int_0x50
        .globl  .add_TIM

        .area   _HEADER_NESTED_TIM (ABS)

        .org    0x50            ; TIM
.int_TIM_nested:
        EI
        PUSH    AF
        PUSH    HL
        LD      HL, #.int_0x50
        JP      .int

        .area   _HOME

_add_low_priority_TIM::
         JP      .add_TIM
