        .include        "global.s"

        .title  "INT Handler"
        .module INTHandler
        .area   _HOME

_INT_ISR::
        push af
        in a, (.VDP_STAT)
        and #.STATF_INT_VBL
        jr z, 1$
        ;; handle VBlank
        push bc
        push de
        push hl
        push iy
        push ix
        
        
        pop ix
        pop iy
        pop hl
        pop de
        pop bc
        pop af
        ei
        reti        
        ;; handle HBlank
1$:     
        push hl
        
        ld hl, (_HBLANK_HANDLER)
        CALL_HL

        pop hl
        pop af
        ei
        reti
        
empty_function:      
        ret
        
        .area   _INITIALIZED
        
_HBLANK_HANDLER::        
        .ds     0x02
        
        .area   _INITIALIZER
        
        .dw     empty_function