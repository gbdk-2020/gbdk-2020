        .include "global.s"

        .title  "EMU_debug"
        .module EMU_debug
        
        .area   _EMU_HEADER (ABS)

        .org    0x08
        JP      _EMU_profiler_message

        .area   _HOME
    
        ;; EMU profiler message
_EMU_profiler_message::
        LD      D, D
        JR      1$
        .dw     0x6464
        .dw     0
        .ascii "PROFILE,%(SP+$0)%,%(SP+$1)%,%A%,%TOTALCLKS%,%ROMBANK%,%WRAMBANK%"
1$:     RET
