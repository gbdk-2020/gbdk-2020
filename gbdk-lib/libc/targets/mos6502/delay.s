.include "global.s"

.title  "Delay"
.module delay

.area _HOME
.bndry 0x20

;
; NTSC:  Delay 1,789,773 / 1000 ~= 1790 cycles -> Y = #252 (24 cycles code cost)
; Dendy: Delay 1,773,448 / 1000 ~= 1773 cycles -> Y = #250 (22 cycles code cost)
; PAL:   Delay 1,662,607 / 1000 ~= 1663 cycles -> Y = #234 (25 cycles code cost)
;
__delay_1ms::
.delay_1ms::
    bit *__SYSTEM
    bmi .delay_1ms_dendy
    bvs .delay_1ms_pal
.delay_1ms_ntsc:
    ldy #252
    bne .delay_1ms_loop
.delay_1ms_pal:
    ldy #234
    bne .delay_1ms_loop
.delay_1ms_dendy:
    ldy #250
    bne .delay_1ms_loop

.delay_1ms_loop:
    nop
    dey
    bne .delay_1ms_loop
    rts

;
; Delay AX milliseconds
; 
; Entry conditions
;   AX = number of milliseconds to delay (1 to 65535)
; 
; Registers used: A, X, Y
;
; Single iteration with X = 0 takes 6 + .delay_1ms + 2 + 2 + 2 + 2 + 3 + 6 = .delay_1ms + 23 cycles 
;       = (((.delay_1ms + 7) * A - 1) + 9) * X + 12
;
_delay::
.delay::
1$: 
2$:
    jsr .delay_1ms
    sec
    sbc #1
    bcs 2$
    cpx #0
    beq 3$
    dex
    jmp 1$
3$:
    rts
