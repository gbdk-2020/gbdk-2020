        .include        "global.s"

        .globl .vdp_shift

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _INITIALIZED

__map_tile_offset::
        .ds     0x01

        .area   _INITIALIZER

        .db     0x00

        .area   _HOME

        ;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_tile_map_xy_compat::
        push hl
        ld hl, #.VDP_TILEMAP

        ;; Set background tile from (BC) at YX = DE, size WH on stack, to VRAM from address (HL)
.set_tile_map_xy_tt_compat::
        push bc                 ; Store source

        ld a, d
        rrca                    ; rrca(2) == rlca(6)
        rrca 
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
        ld c, a                 ; dest BC = HL + ((0x20 * Y) * 2) + (X * 2)

        ld a, b
        cp #>(.VDP_TILEMAP+0x0700)
        jr c, 5$
        ld b, #>.VDP_TILEMAP
5$:
        pop hl                  ; HL = source
        pop de                  ; DE = HW
        push ix                 ; save IX
        push de                 ; store HW
        ld ixh, b
        ld ixl, c
        push ix                 ; store dest

        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT

1$:                             ; copy H rows
        VDP_WRITE_CMD ixh, ixl
        ld c, #.VDP_DATA
2$:                             ; copy W tiles
        ld a, (__map_tile_offset)
        add (hl)
        out (c), a
        inc hl
        dec b
        jr 8$                   ; delay
8$:
        in a, (c)               ; skip next byte

        ld a, ixl
        and #0x3F
        inc a
        inc a
        bit 6, a
        jp z, 3$
        and #0x3F
        ld b, a
        ld a, ixl
        and #0xC0
        or b
        ld ixl, a
        VDP_WRITE_CMD ixh, ixl
        dec e
        jp nz, 2$
        jp 7$
3$:
        inc ixl
        inc ixl
        dec e
        jp nz, 2$
7$:
        pop ix
        pop de

        dec d
        jr z, 6$

        push de

        ld bc, #0x40
        add ix, bc
        ld a, ixh
        cp #>(.VDP_TILEMAP+0x0700)
        jp c, 4$
        ld ixh, #>.VDP_TILEMAP
4$:        
        push ix
        jp 1$
6$:
        ENABLE_VBLANK_COPY      ; switch ON copy shadow SAT
        pop ix                  ; restore IX
        ret