        .include        "global.s"

        .module nested_timer_interrupts

        .area   _HEADER_NESTED_TIM (ABS)

        .org    0x50            ; TIM
.int_TIM_nested:
        ei
        push af
        push hl
        push bc
        push de
        jp .int_0x50

        .area   _HOME

_add_low_priority_TIM::
        jp .add_TIM
