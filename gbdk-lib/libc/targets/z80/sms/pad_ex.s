        .include        "global.s"

        .title  "JOYPad utilities"
        .module JOYPad
        .area   _HOME

; uint8_t joypad_init(uint8_t npads, joypads_t * joypads) __z88dk_callee;
_joypad_init::
        ld hl, #2
        add hl, sp
        ld a, (hl)
        or a
        jr nz, 1$
        ld a, #1
        jr 2$
1$:
        cp #(.JOYPAD_COUNT + 1)
        jr c, 2$
        ld a, #.JOYPAD_COUNT
2$:
        inc hl
        ld e, (hl)
        inc hl
        ld d, (hl)

        inc hl
        pop bc          ; pop return address
        ld sp, hl       ; dispose params
        push bc         ; push return address back

        ld (de), a      ; number of joypads
        ld l, a         ; return number of joypads in l
        xor a
        inc de
        ld (de), a
        inc de
        ld (de), a
        inc de
        ld (de), a
        inc de
        ld (de), a

        ret

;void joypad_ex(joypads_t * joypads) __z88dk_fastcall;

_joypad_ex::
        inc hl                  ; hl = joypads[]
        ld c, #.JOY_PORT1
        in e, (c)
        inc c
        in d, (c)
        ld a, e
        rla
        rl d
        rla
        rl d
        ld a, e
        cpl
        and #(.JOY_P1_UP | .JOY_P1_DOWN | .JOY_P1_LEFT | .JOY_P1_RIGHT | .JOY_P1_SW1 | .JOY_P1_SW2)
        ld e, a
        and #(.JOY_P1_SW1 | .JOY_P1_SW2)
        rlca
        rlca
        or e
        ld (hl), a              ; joypads[0] = buttons1
        inc hl

        ld a, d
        cpl
        and #(.JOY_P1_UP | .JOY_P1_DOWN | .JOY_P1_LEFT | .JOY_P1_RIGHT | .JOY_P1_SW1 | .JOY_P1_SW2)
        ld d, a
        and #(.JOY_P1_SW1 | .JOY_P1_SW2)
        rlca
        rlca
        or d
        ld (hl), a              ; joypads[1] = buttons2

        ret