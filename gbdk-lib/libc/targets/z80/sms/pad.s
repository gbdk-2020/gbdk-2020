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

        ld a, #.JOY_TH_LO
        out (.JOY_CTL), a

        in a, (.JOY_PORT1)
        ld h, a
        and #(.JOY_P1_LEFT | .JOY_P1_RIGHT)
        jp nz, 1$

        ld a, h                 ; three button controller detected
        cpl
        ld h, a
        and #.JOY_P1_MD_START   ; .START = .JOY_P1_MD_START << 1
        rlca
        or l
        ld l, a
        ld a, h
        and #.JOY_P1_MD_A       ; .SELECT = .JOY_P1_MD_A << 3
        rlca
        rlca
        rlca
        or l
        ld l, a

        .rept 3                 ; skip 6-button controller keys
            ld a, #.JOY_TH_HI
            out (.JOY_CTL), a
            ld a, #.JOY_TH_LO
            out (.JOY_CTL), a
        .endm

        ld a, #.JOY_TH_HI
        out (.JOY_CTL), a
        ret
1$:
        ld a, l                 ; copy buttons 1 and 2 into start and select
        and #(.JOY_P1_SW1 | .JOY_P1_SW2)
        rlca
        rlca
        or l                    ; .START = .JOY_P1_SW1, .SELECT = .JOY_P1_SW2
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
