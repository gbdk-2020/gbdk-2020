.include "global.s"

    .area   _HOME

__switch_prg0::
    sta *__current_bank
    tay
    sta .identity,Y
    rts
