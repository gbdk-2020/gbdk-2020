        .include        "global.s"

        .title  "NMI Handler"
        .module NMIHandler
        .area   _HOME

_NMI_ISR::
        retn