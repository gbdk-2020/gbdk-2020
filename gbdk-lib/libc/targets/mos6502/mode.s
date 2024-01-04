    .include    "global.s"

    .title  "screen modes"
    .module Modes

    .area   GBDKOVR (PAG, OVR)
    temp_word:                  .ds 2

    ;; BANKED:  checked
    .area   _HOME

_mode::
.set_mode::
    sta .mode
    ;; AND to get rid of the extra flags
    and #0x03
    asl                     ; Multiply mode by 4
    asl
    clc
    adc #<.MODE_TABLE
    sta temp_word
    lda #0
    adc #>.MODE_TABLE
    sta temp_word+1
    jmp [*temp_word]        ; Jump to initialization routine

_get_mode::
    lda .mode
    rts
