        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

	;; Get Keypad Button Status
_joypad::
.jpad::
        in a, (#.JOY_PORT1)
        cpl
        and #0b00111111
        ld l, a
        ret

	;; Wait until all buttons have been released
.padup::
_waitpadup::
        push hl         ; Save modified registers
        push de
1$:
        ld d,#0x7f      ; wait for .jpad return zero 127 times in a row
2$:
        call .jpad
        or a            ; have all buttons been released?
        jr nz,1$        ; not yet

        dec d
        jr nz,2$
        
        pop de
        pop hl
        ret

        ;; Wait for the key to be pressed
_waitpad::
        ld hl, #2
        add hl, sp
        ld h, (hl)
        ;; wait for the key in d to be pressed
.wait_pad::
1$:
        call .jpad      ; read pad
        and h           ; compare with mask?
        jr z,1$         ; loop if no intersection
        ret
