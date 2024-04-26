        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

	;; Get Keypad Button Status
_joypad::
.jpad::
        in a, (.JOY_PORT1)
        cpl
        and #(.JOY_P1_UP | .JOY_P1_DOWN | .JOY_P1_LEFT | .JOY_P1_RIGHT | .JOY_P1_SW1 | .JOY_P1_SW2)
        ld l, a
        and #.JOY_P1_SW2
        rlca
        rlca
        or l            ; .SELECT = .JOY_P1_SW2
        ld l, a
        in a, (.GG_STATE)
        cpl
        and #.GGSTATE_STT
        rrca
        or l            ; .START = .GGSTATE_STT
        ld l, a
        ret

	;; Wait until all buttons have been released
.padup::
_waitpadup::
1$:
        ld h, #0x7f     ; wait for .jpad return zero 127 times in a row
2$:
        call .jpad
        or a            ; have all buttons been released?
        jr nz, 1$       ; not yet

        dec h
        jr nz, 2$

        ret

        ;; Wait for the key to be pressed
_waitpad::
.wait_pad::
        ld h, l
1$:
        call .jpad      ; read pad
        and h           ; compare with mask?
        jr z, 1$        ; loop if no intersection
        ret
