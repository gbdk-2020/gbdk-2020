        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

	;; Get Keypad Button Status
_joypad::
.jpad::
        in a, (.JOY_PORT1)
        cpl
        and #0b00111111
        ld l, a
        ret

	;; Wait until all buttons have been released
.padup::
_waitpadup::
1$:
        ld h,#0x7f      ; wait for .jpad return zero 127 times in a row
2$:
        call .jpad
        or a            ; have all buttons been released?
        jr nz,1$        ; not yet

        dec h
        jr nz,2$
        
        ret

        ;; Wait for the key to be pressed
_waitpad::
.wait_pad::
1$:
        ld h, l
1$:
        call .jpad      ; read pad
        and h           ; compare with mask?
        jr z,1$         ; loop if no intersection
        ret
