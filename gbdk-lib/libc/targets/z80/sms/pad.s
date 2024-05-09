        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

; Get Keypad Button Status
; return result in l
;
;uint8_t joypad(void) OLDCALL PRESERVES_REGS(b, c, d, e, iyh, iyl);
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

; wait for 1/4 frame for the MD 3 and 6-button controllers
do_sleep:
        push bc
        ld c, #10
0$:
        ld b, #0xff
1$:
        .rept 4
            ex (sp), hl
        .endm
        djnz 1$

        dec c
        jr nz, 0$
        pop bc
        ret

; Wait until all buttons have been released
;
; void waitpadup(void) PRESERVES_REGS(d, e, iyh, iyl);
.padup::
_waitpadup::
1$:
        call .jpad
        ld a, l
        or a            ; have all buttons been released?
        ret z
        call do_sleep   ; wait 1/4 frame
        jr 1$           ; not yet

; Wait for the key to be pressed
;
; uint8_t waitpad(uint8_t mask) Z88DK_FASTCALL PRESERVES_REGS(d, e, iyh, iyl);
_waitpad::
.wait_pad::
        ld c, l
1$:
        call .jpad      ; read pad
        ld a, l
        and c           ; compare with mask?
        ret nz          ; return if intersection
        call do_sleep   ; wait 1/4 frame
        jr 1$
