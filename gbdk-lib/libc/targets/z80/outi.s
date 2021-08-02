        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

.OUTI128::                              ; _OUTI128 label points to a block of 128 OUTI and a RET
        .rept 64
                outi
        .endm
.OUTI64::                               ; _OUTI64 label points to a block of 64 OUTI and a RET
        .rept 32
                outi
        .endm
.OUTI32::                               ; _OUTI32 label points to a block of 32 OUTI and a RET
        .rept 32
                outi
        .endm
.OUTI_END::                             ; _outi_block label points to END of OUTI block
        ret
