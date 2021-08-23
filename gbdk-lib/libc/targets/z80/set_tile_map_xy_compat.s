        .include        "global.s"

        .globl .vdp_shift

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

        ;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_tile_map_xy_compat::
        push hl
        ld hl, #.VDP_TILEMAP

        ;; Set background tile from (BC) at YX = DE, size WH on stack, to VRAM from address (HL)
.set_tile_map_xy_tt_compat::
        push bc         ; Store source

        ld a, d
        rlca 
        rlca 
        rlca 
        rlca 
        rlca 
        rlca 
        ld d, a
        and #0x07
        add h
        ld b, a
        ld a, #0xC0
        and d
        sla e
        add e
        ld hl, #.vdp_shift
        add (hl)
        ld c, a         ; dest BC = HL + ((0x20 * Y) * 2) + (X * 2)

        ld a, b
        cp #>(.VDP_TILEMAP+0x0700)
        jr c, 5$
        ld b, #>.VDP_TILEMAP
5$:
        dec bc

        pop hl          ; HL = source
        pop de          ; DE = HW
        push de         ; store HW
        push bc         ; store dest

        DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

1$:                     ; copy H rows
        SMS_WRITE_VDP_CMD b, c
        ld c, #.VDP_DATA
        ld b, e
2$:                     ; copy W tiles
        in a, (c)       ; skip next byte
        nop
        jr 3$
3$:
        outi
        nop
        jr nz, 2$

        pop bc
        pop de
        dec d
        jr z, 6$

        push de

        ld a, #0x40
        add c
        ld c, a
        adc b
        sub c
        ld b, a

        cp #>(.VDP_TILEMAP+0x0700)
        jr c, 4$
        ld b, #>.VDP_TILEMAP
4$:        
        push bc
        jr 1$
6$:
        ENABLE_VBLANK_COPY        ; switch ON copy shadow SAT

        ret