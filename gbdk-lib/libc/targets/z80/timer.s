        .include        "global.s"

        .title  "Timer dispatcher"
        .module TIMDispatcher

        .globl  .add_int, .remove_int
    .globl  .TIM_DISPATCHER

        .area   _HOME

LowPri_TIM_Dispatcher:
        ld hl, #_TIMA_REG
        inc (hl)
        ret nz
        ei
        jp .TIM_continue
TIM_Dispatcher:
        ld hl, #_TIMA_REG
        inc (hl)
        ret nz
.TIM_continue:
        ld a, (_TMA_REG)
        ld (hl), a

        .irp idx,.TIM_HANDLER0,.TIM_HANDLER1,.TIM_HANDLER2
                ld hl, (idx)
                ld a, h
                or l
                jp z, 1$
                CALL_HL
        .endm
1$:
        ret

_remove_TIM::
        ld b, h
        ld c, l
        ld hl, #.TIM_HANDLER0
        jp .remove_int

_add_low_priority_TIM::
        ld b, h
        ld c, l
        ld hl, #LowPri_TIM_Dispatcher
        jr .add_TIM
_add_TIM::
        ld b, h
        ld c, l
        ld hl, #TIM_Dispatcher
.add_TIM:
        ld (.TIM_DISPATCHER), hl
        ld hl, #.TIM_HANDLER0
        jp .add_int

        .area   _DATA

_TAC_REG::
        .ds     0x01

        .area   _INITIALIZED

_TIMA_REG::
        .ds     0x01
_TMA_REG::
        .ds     0x01
.TIM_HANDLER0:
        .ds     0x02
.TIM_HANDLER1:
        .ds     0x02
.TIM_HANDLER2:
        .ds     0x02
        .ds     0x02

        .area   _INITIALIZER

        .db     0xff
        .db     0xff
        .dw     0x0000
        .dw     0x0000
        .dw     0x0000
        .dw     0x0000