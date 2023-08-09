.include "global.s"
.include "mapper_macros.s"

__switch_prg0::
    sta *__current_bank
    SWITCH_PRG0_A
    rts
