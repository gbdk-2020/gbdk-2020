        .include "global.s"

        .globl  .set_xy_tt

        .area   _HOME

; void set_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *vram_addr, const uint8_t *tiles);

_set_tiles::
        PUSH    BC
        CALL    1$
        POP     BC
        RET
1$:
        LDHL    SP, #9
        LD      A, (HL-)
        LD      D, A
        LD      A, (HL-)
        LD      E, A
        PUSH    DE
        LD      A, (HL-)
        LD      E, A
        LD      D, (HL)
        LDHL    SP, #15
        LD      A, (HL-)
        LD      B, A
        LD      A, (HL-)
        LD      C, A
        LD      A, (HL-)
        LD      L, (HL)
        LD      H, A

        JP      .set_xy_tt      ;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (HL)
