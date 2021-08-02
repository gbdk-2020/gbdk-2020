        .include        "global.s"

        .title  "INT Handler"
        .module INTHandler
        .area   _HOME

_INT_ISR::
        ei
        reti