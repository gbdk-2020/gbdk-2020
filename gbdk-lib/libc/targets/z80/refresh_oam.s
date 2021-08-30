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
        SMS_WRITE_VDP_CMD d, e

        ld h, #>_shadow_OAM
        ld l, #0
        ld c, #.VDP_DATA
        ld b, #64
1$:
        outi
        jp nz, 1$

        ld de, #(.VDP_SAT + 0x80)
        SMS_WRITE_VDP_CMD d, e
        ld b, #128
2$:
        outi
        jp nz, 2$

        ENABLE_VBLANK_COPY      ; switch OFF copy shadow SAT
        ret