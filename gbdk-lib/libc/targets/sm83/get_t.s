        .include "global.s"

        .area   _HOME

; void get_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *vram_addr, uint8_t *tiles);

_get_tiles::
        LDHL    SP, #5
        LD      A, (HL-)
        LD      E, A
        LD      A, (HL-)
        LD      D, A
        PUSH    DE
        LD      A, (HL-)
        LD      E, A
        LD      D, (HL)
        LDHL    SP, #11
        LD      A, (HL-)
        LD      B, A
        LD      A, (HL-)
        LD      C, A
        LD      A, (HL-)
        LD      L, (HL)
        LD      H, A

        JP      .get_xy_tt      ;; Store background tile table into (BC) at XY = DE, size WH on stack, from vram from address (HL)
