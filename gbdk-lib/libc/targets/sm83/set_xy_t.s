        .include        "global.s"

        .title  "Set tile map"
        .module SetTileMap

        .area   _INITIALIZED

__map_tile_offset::
        .ds     0x01

        .area   _INITIALIZER

        .db     0x00

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
        LD      HL,#0x9800
        JR      .set_xy_tt
.is9c:
        LD      HL,#0x9C00
        ;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (HL)
.set_xy_tt::
        PUSH    BC              ; Store source

        SWAP    E
        RLC     E
        LD      A,E
        AND     #0x03
        ADD     H
        LD      B,A
        LD      A,#0xE0
        AND     E
        ADD     D
        LD      C,A             ; dest BC = HL + 0x20 * Y + X

        POP     HL              ; HL = source
        POP     DE              ; DE = WH
        PUSH    DE              ; store WH
        PUSH    BC              ; store dest

3$:                             ; Copy W tiles

        WAIT_STAT
        LD      A, (__map_tile_offset)
        ADD     (HL)
        LD      (BC), A
        INC     HL
        
        LD      A, C            ; inc dest and wrap around
        AND     #0xE0
        LD      E, A
        LD      A, C
        INC     A
        AND     #0x1F
        OR      E
        LD      C, A

        DEC     D
        JR      NZ, 3$

        POP     BC
        POP     DE

        DEC     E
        RET     Z

        PUSH    DE

        LD      A, B            ; next row and wrap around
        AND     #0xFC
        LD      E, A            ; save high bits

        LD      A,#0x20

        ADD     C
        LD      C, A
        ADC     B
        SUB     C
        AND     #0x03
        OR      E               ; restore high bits
        LD      B, A

        PUSH    BC
        
        JR      3$
