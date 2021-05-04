        .include "global.s"

        .globl  .set_xy_tt

	;; BANKED: checked, imperfect
        .area   _BASE

; void set_tiles(UINT8 x, UINT8 y, UINT8 w, UINT8 h, unsigned char *vram_addr, const unsigned char *tiles);

_set_tiles::
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

        CALL    .set_xy_tt      ;; Set background tile from (BC) at XY = DE, size WH on stack, to vram from address (HL)

        POP     BC
        RET
