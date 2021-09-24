        .include        "global.s"

        .area   _HOME

        ;; Initialize window tile table with B
.init_wtt::
        LDH     A,(.LCDC)
        AND     #LCDCF_WIN9C00
        JR      Z,.is98
        JR      .is9c
        ;; Initialize background tile table with B
.init_btt::
        LDH     A,(.LCDC)
        AND     #LCDCF_BG9C00
        JR      NZ,.is9c
.is98:
        LD      HL,#0x9800      ; HL = origin
        JR      .init_tt
.is9c:
        LD      HL,#0x9C00      ; HL = origin

.init_tt::
        LD      DE,#0x0400      ; One whole GB Screen

.init_vram::
        SRL     D
        RR      E
        JR      NC, 1$

        WAIT_STAT
        LD      A, B
        LD      (HL+),A
1$:
        LD      A, D
        OR      E
        RET     Z
        
        INC     D
        INC     E
        JR      2$
3$:
        WAIT_STAT
        LD      A, B
        LD      (HL+),A
        LD      (HL+),A
2$:
        DEC     E
        JR      NZ, 3$
        DEC     D
        JR      NZ, 3$
        
        RET
