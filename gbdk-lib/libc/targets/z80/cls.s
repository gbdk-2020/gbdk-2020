        .include        "global.s"

        .title  "console utilities"
        .module ConsoleUtils

        .area   _HOME

_cls::
        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT
        ld hl, #(.VDP_TILEMAP + ((.SCREEN_Y_OFS * .VDP_MAP_WIDTH) * 2))
        WRITE_VDP_CMD_HL

        ld bc, #(.SCREEN_HEIGHT * .VDP_MAP_WIDTH)
        inc b
        inc c
        jr 1$
2$:
        ld a, #.SPACE
        out (.VDP_DATA), a
        VDP_DELAY
        xor a
        out (.VDP_DATA), a
1$:
        dec c
        jr nz, 2$
        dec b
        jr nz, 2$ 

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT
        ret