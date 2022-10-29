        .include        "global.s"

        .area   _CODE

        ;; Check if running on SGB
        ;;   Set A to 0xFF when running on SGB
        ;;   Clear A when running on DMG
.sgb_check::
_sgb_check::
        PUSH    BC
        ADD     SP, #-0x10
        LDHL    SP, #0
        LD      C, #0x10
        XOR     A
        RST     0x28
        LDHL    SP,#0
        LD      A,#((.MLT_REQ << 3) | 1)
        LD      (HL+),A
        LD      A,#0x03
        LD      (HL-),A
        CALL    .sgb_transfer
        LD      C,#.P1
        LD      A,#(.P14 | .P15)
        LDH     (C),A
        LDH     A,(C)
        LDH     A,(C)
        LD      E, #4
        LDH     A,(C)           ; read delayed
3$:
        LD      B, A

        LD      A,#.P15
        LDH     (C),A
        LDH     A,(C)
        LDH     A,(C)

        LD      A,#.P14
        LDH     (C),A
        LDH     A,(C)
        LDH     A,(C)

        LD      A,#(.P14 | .P15)
        LDH     (C),A
        LDH     A,(C)
        LDH     A,(C)
        LDH     A,(C)
        LDH     A,(C)           ; read delayed
        CP      B
        JR      NZ,1$
        
        DEC     E
        JR      NZ,3$
2$:
        LD      E,#0
        JR      4$
1$:
        LDHL    SP,#1
        LD      A,#0x00
        LD      (HL-),A
        CALL    .sgb_transfer
        LD      E,#1
4$:
        ADD     SP,#0x10
        POP     BC
        RET

_sgb_transfer::
        LDHL    SP,#2
        LD      A,(HL+)
        LD      H,(HL)
        LD      L,A
        
.sgb_transfer::
        PUSH    BC
        LD      A,(HL)          ; Top of command data
        AND     #0x03
        JR      Z,6$
1$:
        PUSH    AF
        LD      C,#.P1
        LDH     (C),A           ; Send reset
        LD      A,#(.P14 | .P15)
        LDH     (C),A
        LD      B,#0x10         ; Set counter to transfer 16 byte
2$:     LD      E,#0x08         ; Set counter to transfer 8 bit
        LD      A,(HL+)
        LD      D,A

3$:
        SRL     D
        LD      A,#.P14         ; P14 = high, P15 = low  (output "1")
        JR      C,4$
        LD      A,#.P15         ; P14 = low,  P15 = high (output "0")
4$:
        LDH     (C),A
        LDH     A,(C)           ; delay
        LDH     A,(C)
        LD      A,#(.P14 | .P15); P14 = high, P15 = high
        LDH     (C),A
        LDH     A,(C)           ; delay
        LDH     A,(C)
        DEC     E
        JR      NZ,3$

        DEC     B
        JR      NZ,2$
        
        LD      A,#.P15         ; 129th bit "0" output
        LDH     (C),A
        LDH     A,(C)           ; delay
        LDH     A,(C)
        LD      A,#(.P14 | .P15)
        LDH     (C),A

        LD      DE,#8400		; was: 7000
5$:
        LDH     A,(.P1)         ; 3 +
        DEC     DE              ; 2 +
        LD      A,D             ; 1 +
        OR      E               ; 1 +
        JR      NZ,5$           ; 3 = 10 cycles

        POP     AF
        DEC     A
        JR      NZ, 1$
6$:
        POP     BC
        RET
