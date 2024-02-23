        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .globl __shadow_OAM_base

        .ez80

        .area   _HOME

; void refresh_OAM(); 
_refresh_OAM::
        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT

        ld de, #.VDP_SAT
        VDP_WRITE_CMD d, e

        ld h, #>_shadow_OAM
        ld l, #0
        ld c, #.VDP_DATA
        ld b, #128
1$:
        outi
        jp nz, 1$

        ENABLE_VBLANK_COPY      ; switch OFF copy shadow SAT
        ret