        .include "global.s"

        ;; BANKED: checked
        .area   _BASE

; void get_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, unsigned char *vram_addr, unsigned char *tiles);

_get_tiles::
        PUSH    BC

        LDHL    SP, #7
        LD      A, (HL-)
        LD      D, A
        LD      A, (HL-)
        LD      E, A
        PUSH    DE
        LD      A, (HL-)
        LD      E, A
        LD      D, (HL)
        LDHL    SP, #13
        LD      A, (HL-)
        LD      B, A
        LD      A, (HL-)
        LD      C, A
        LD      A, (HL-)
        LD      L, (HL)
        LD      H, A

        CALL    L.get_xy_tt      ;; Store background tile table into (BC) at XY = DE, size WH on stack, from vram from address (HL)

        POP     BC
        RET
