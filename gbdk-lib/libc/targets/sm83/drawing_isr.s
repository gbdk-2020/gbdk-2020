        .include	"global.s"

        .area	_HOME

.drawing_lcd::
        ld hl, #rLCDC
        ldh a, (rLYC)
        cp #72
        jr z, 1$
        set LCDCF_B_BG8000, (hl)
        ld a, #72
        ldh (rLYC), a
        ret
1$:
        WAIT_STAT
        res LCDCF_B_BG8000, (hl)
        ld a, #144
        ldh (rLYC), a
        ret
