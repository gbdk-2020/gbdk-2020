        .include        "global.s"

        .title  "console utilities"
        .module ConsoleUtils

        .globl  .curx, .cury

        .area   _HOME

_scroll_viewport::
        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT
        ld hl, #(.VDP_TILEMAP + ((.SCREEN_Y_OFS + 1) * .VDP_MAP_WIDTH * 2))

        ld bc, #((.SCREEN_HEIGHT - 1) * .VDP_MAP_WIDTH * 2)
        inc b
        inc c
        jr 1$
2$:
        res 6, h
        WRITE_VDP_CMD_HL
        nop
        nop
        in a, (.VDP_DATA)
        ld e, a

        ld a, l
        sub #(.VDP_MAP_WIDTH * 2)
        ld l, a
        ld a, h
        sbc #0
        ld h, a

        set 6, h
        WRITE_VDP_CMD_HL
        nop
        ld a, e
        out (.VDP_DATA), a

        ld a, #((.VDP_MAP_WIDTH * 2) + 1)
        add l
        ld l, a
        adc h
        sub l
        ld h, a

1$:
        dec c
        jr nz, 2$
        djnz 2$ 

        ld hl, #(.VDP_TILEMAP + ((.SCREEN_Y_OFS + .SCREEN_HEIGHT - 1) * .VDP_MAP_WIDTH * 2))
        WRITE_VDP_CMD_HL

        ld hl, #.SPACE
        ld b, #.VDP_MAP_WIDTH
3$:
        WRITE_VDP_DATA_HL
        djnz 3$

        ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT
        ret