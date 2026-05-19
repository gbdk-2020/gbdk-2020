        .include        "global.s"

        .title  "Set tile map"
        .module SetTileMap

        .area   _DATA

__map_tile_offset::
        .ds     0x01

        .area   _HOME

        ;; Set window tile table from BC at XY = DE of size WH = HL
.set_xy_wtt::
        PUSH    HL              ; Store WH
        LDH     A,(.LCDC)
        AND     #LCDCF_WIN9C00
        JR      Z,.is98
        JR      .is9c
        ;; Set background tile table from (BC) at XY = DE of size WH = HL
.set_xy_btt::
        PUSH    HL              ; Store WH
        LDH     A,(.LCDC)
        AND     #LCDCF_BG9C00
        JR      NZ,.is9c
.is98:
        ld h, #0x98
        jr .set_xy_tt
.is9c:
        ld h, #0x9C
        ;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (H << 8)
.set_xy_tt::
        ld l, d
        swap e
        ld a, e
        res 0, e
        and #1
        ld d, a
        add hl, de
        add hl, de            ; dest DE = (H << 8) + 0x20 * Y + X
        
        ld d, h
        ld e, l                

        ld h, b
        ld l, c
        
        pop bc                 ; get WH
0$:     
        push bc                ; store WH
        push de                ; store dest

        ld a, (__map_tile_offset)
        ld c, a
1$:
        WAIT_STAT
        ld a, (hl+)
        add c
        ld (de), a

        inc e
        ld a, e
        and #0x1F
        jr nz, 2$
        ld a, e
        sub #0x20
        ld e, a
2$:
        dec b
        jr nz, 1$
        
        pop de
        pop bc

        dec c
        ret z

        ; next row and wrap around
        ld a, e
        add #0x20
        ld e, a
        jr nc, 0$
        ld a, d
        rrca
        rrca
        add #0x40
        rlca
        rlca
        ld d, a
        
        jr 0$
