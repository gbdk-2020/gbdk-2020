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
        in a, (.GG_STATE)
        cpl
        and #.GGSTATE_STT
        ld b, a
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
        and #0b00111111
        or b
        ld e, a
        ld a, d
        cpl
        and #0b00111111
        ld d, a
        
        inc hl
        ld (hl), e
        inc hl
        ld (hl), d
        
        ret