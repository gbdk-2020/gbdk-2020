        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

	;; Get Keypad Button Status
_joypad::
.jpad::
        ld a, #0x06     ; ... <code> <caps> <graph> <ctrl> <shift>
        out (.KBD_SELECT_ROW), a
        in a, (.KBD_INPUT)
        cpl

        rrca
        rl l
        rrca
        rrca
        rl l            ; shift graph

        ld a, #0x07     ; <ret>, ...
        out (.KBD_SELECT_ROW), a
        in a, (.KBD_INPUT)
        cpl

        rlca
        rl l
        rl l
        ld a, #0b00001110
        and l
        ld l, a

        ld a, #0x08     ; <r><d><u><l><del><ins><home><space>
        out (.KBD_SELECT_ROW), a
        in a, (.KBD_INPUT)
        cpl
        and #0b11110001

        or l
        ld l, a         ; <r><d><u><l><shift><graph><enter><space>

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
        ld h, l
1$:
        call .jpad      ; read pad
        and h           ; compare with mask?
        jr z,1$         ; loop if no intersection
        ret
