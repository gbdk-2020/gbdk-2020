        .include "global.s"

        .globl  .set_xy_tt

        .area   _HOME

; void set_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *vram_addr, const uint8_t *tiles);

_set_tiles::
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
        LD      H, (HL)         ; only load the high byte of tiles since its lower byte will be ignored by .set_xy_t anyway

        JP      .set_xy_tt      ;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (H << 8)
