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
        LD      HL,#0x9800      ; HL = origin
        JR      .fill_rect
.is9c:
        LD      HL,#0x9C00      ; HL = origin

        ;; fills rectangle area with tile B at XY = DE, size WH on stack, to vram from address (HL)
.fill_rect:
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

        POP     HL              ; H = Tile
        POP     DE              ; DE = WH
        PUSH    DE              ; store WH
        PUSH    BC              ; store dest

3$:                             ; Copy W tiles

        WAIT_STAT
        LD      A, H
        LD      (BC), A
        
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
