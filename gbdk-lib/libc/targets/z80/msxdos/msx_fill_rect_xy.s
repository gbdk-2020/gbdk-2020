        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _HOME

        ;; Set background tile table from (BC) at XY = DE of size WH = HL
.fill_rect_xy::
        push hl
        ld hl, #.VDP_TILEMAP

        ;; Set background tile from (BC) at YX = DE, size WH on stack, to VRAM from address (HL)
.fill_rect_xy_tt::
        push bc                 ; Store source

        ld a, d
        rrca                    ; rrca(3) == rlca(5)
        rrca 
        rrca 
        ld d, a
        and #0x07
        add h
        ld b, a
        ld a, #0xE0
        and d
        add e
        ld c, a                 ; dest BC = HL + ((0x20 * Y) + X

        ld a, b
        cp #>(.VDP_TILEMAP+0x0300)
        jr c, 5$
        ld b, #>.VDP_TILEMAP
5$:

        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT

        pop hl                  ; HL = source
        pop de                  ; DE = HW
        push ix                 ; save IX
        push de                 ; store HW
        ld ixh, b
        ld ixl, c
        push ix                 ; store dest

1$:                             ; copy H rows
        VDP_WRITE_CMD ixh, ixl
        ld c, #.VDP_DATA
2$:                             ; copy W tiles
        out (c), l

        ld a, ixl
        and #0x1F
        inc a
        bit 5, a
        jp z, 3$
        and #0x1F
        ld b, a
        ld a, ixl
        and #0xE0
        or b
        ld ixl, a
        VDP_WRITE_CMD ixh, ixl
        dec e
        jp nz, 2$
        jp 7$
3$:
        inc ixl
        dec e
        jp nz, 2$
7$:
        pop ix
        pop de

        dec d
        jr z, 6$

        push de

        ld bc, #0x20
        add ix, bc
        ld a, ixh
        cp #>(.VDP_TILEMAP+0x0300)
        jp c, 4$
        ld ixh, #>.VDP_TILEMAP
4$:        
        push ix
        jp 1$
6$:
        ENABLE_VBLANK_COPY      ; switch ON copy shadow SAT
        pop ix                  ; restore IX
        ret