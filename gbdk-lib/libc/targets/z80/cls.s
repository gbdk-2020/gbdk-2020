        .include        "global.s"

        .title  "console utilities"
        .module ConsoleUtils

        .globl  .curx, .cury

        .area   _HOME

_cls::
        ld a, #.SCREEN_X_OFS
        ld (.curx), a
        ld a, #.SCREEN_Y_OFS
        ld (.cury), a
        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT
        ld hl, #(.VDP_TILEMAP + ((.SCREEN_Y_OFS * .VDP_MAP_WIDTH) * 2))
        WRITE_VDP_CMD_HL

        ld hl, #.SPACE
        ld bc, #(.SCREEN_HEIGHT * .VDP_MAP_WIDTH)
        inc b
        inc c
        jr 1$
2$:
        WRITE_VDP_DATA_HL
1$:
        dec c
        jr nz, 2$
        djnz 2$ 

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT
        ret