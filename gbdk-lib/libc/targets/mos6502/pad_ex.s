    .include    "global.s"

    MAX_JOYPADS = 2
    .globl _read_joypad

    .area   OSEG (PAG, OVR)
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
    sta *_joypad_init_PARM_2
    stx *_joypad_init_PARM_2+1
    ; Joypad #0
    ldx #0
    jsr _read_joypad
    ldy #1
    sta [*_joypad_init_PARM_2],y
    ; Joypad #1
    ldx #1
    jsr _read_joypad
    ldy #2
    sta [*_joypad_init_PARM_2],y
    rts
