        .include "global.s"

        .module timer_interrupts

        .area   _HEADER_TIM (ABS)

        .org    0x50            ; TIM
.int_TIM:
        push af
        push hl
        push bc
        push de
        jp .int_0x50

        .area   _HOME

_add_TIM::
        jp .add_TIM
