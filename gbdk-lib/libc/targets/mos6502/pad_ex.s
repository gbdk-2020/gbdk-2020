    .include    "global.s"

    MAX_JOYPADS = 4
    .globl _read_joypad

    .area   GBDKOVR (PAG, OVR)
    _joypad_init_PARM_2::       .ds 2

    .area   _HOME

_joypad_init::
    ; Report at most MAX_JOYPADS number of joypads available
    cmp #MAX_JOYPADS
    bcc 1$
    lda #MAX_JOYPADS
1$:
    pha
    tax
    ldy #0
    sta [*_joypad_init_PARM_2],y
    iny
    lda #0
_joypad_init_loop:
    sta [*_joypad_init_PARM_2],y
    iny
    dex
    bne _joypad_init_loop
    pla
    rts

_joypad_ex::
.joypad_ex::
    .define .joypads_ptr    "___SDCC_m6502_ret0"
    .define .joypads_endof  ".tmp"
    sta *.joypads_ptr
    stx *.joypads_ptr+1
    ; endof = Number of joypads + 1
    ldy #0
    lda [*.joypads_ptr],y
    iny
    sta *.joypads_endof
    inc *.joypads_endof
    ;
    jsr _strobe_joypads
.joypad_ex_loop:
    lda .joypad_ex_lut,y
    tax
    jsr _read_joypad_no_strobe
    sta [*.joypads_ptr],y
    iny
    cpy *.joypads_endof
    bne .joypad_ex_loop
    rts

;
; Look-up table for getting joypad port offset from offset into joypads_t (=joypad index + 1)
;
.joypad_ex_lut:
.db 0
.db 0
.db 1
.db 0
.db 1
