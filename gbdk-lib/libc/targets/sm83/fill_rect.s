        .include        "global.s"

        .area   _HOME

.fill_rect_wtt::
        PUSH    HL
        LDH     A,(.LCDC)
        AND     #LCDCF_WIN9C00
        JR      Z,.is98
        JR      .is9c
        ;; Initialize background tile table with B
.fill_rect_btt::
        PUSH    HL
        LDH     A,(.LCDC)
        AND     #LCDCF_BG9C00
        JR      NZ,.is9c
.is98:
        LD      H, #0x98        ; HL = origin
        JR      .fill_rect
.is9c:
        LD      H, #0x9C        ; HL = origin

        ;; fills rectangle area with tile B at XY = DE, size WH on stack, to vram from address (H << 8)
.fill_rect:
        ld l, d
        swap e
        ld a, e
        res 0, e
        and #1
        ld d, a
        add hl, de
        add hl, de              ; dest HL = (H << 8) + 0x20 * Y + X
        
        pop de                  ; DE = WH
        ld c, d                 ; C = W
0$:
        push hl                 ; store dest
1$:                             ; Copy W tiles
        WAIT_STAT
        ld (hl), b

        ; inc dest and wrap around
        inc l
        ld a, l
        and #0x1F
        jr nz, 2$
        ld a, l
        sub #0x20
        ld l, a
2$:
        dec d
        jr nz, 1$
        
        pop hl

        dec e
        ret z

        ld d, c        ; D = W
        
        ; next row and wrap around
        ld a, l
        add #0x20
        ld l, a
        jr nc, 0$
        ld a, h
        rrca
        rrca
        add #0x40
        rlca
        rlca
        ld h, a
        
        jr 0$
