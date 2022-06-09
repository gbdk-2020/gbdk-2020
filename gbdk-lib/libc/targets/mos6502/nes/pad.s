    .include    "global.s"

    .area   _HOME

_read_joypad::
    ; Strobe to latch joypad data
    lda #1
    sta JOY1
    lda #0
    sta JOY1
    ;
    ldy #8
_read_joypad_loop:
    lda JOY1,x
    lsr
    ror *.tmp+1
    dey
    bne _read_joypad_loop
    lda *.tmp+1
    rts

    ;; Wait until all buttons have been released
.padup::
_waitpadup::
    jsr .jpad
    beq _waitpadup
    rts

    ;; Get Keypad Button Status
_joypad::
.jpad::
    ldx #0
    jmp _read_joypad

    ;; Wait for the key to be pressed
_waitpad::
.wait_pad::
    sta *.tmp
.wait_pad_loop:
    jsr .jpad
    and *.tmp
    beq .wait_pad_loop
    rts
