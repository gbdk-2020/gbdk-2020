        .include        "global.s"

        .title  "Delay"
        .module delay

        .area _CODE

;; hl must contain amount of tstates, which must be >= 141
wait_hl_tstates::
        ld bc, #-141
        add hl, bc
        ld bc, #-23
1$:
        add hl,bc
        jr c, 1$
        ld a, l
        add a, #15
        jr nc, 2$
        cp #8
        jr c, 3$
        or #0
2$:
        inc hl
3$:
        rra
        jr c, 4$
        nop
4$:
        rra
        jr nc, 5$   
        or #0
5$:
        rra
        ret nc
        ret

;; hl = milliseconds (0 = 65536)
_delay::
        ld e, l
        ld d, h
1$:
        dec de
        ld a,d
        or e
        jr z, 2$

        ld hl,#((.CPU_CLOCK / 1000) - 43)
        call wait_hl_tstates
        jr 1$
2$:
        ld hl,#((.CPU_CLOCK / 1000) - 54)
        jp wait_hl_tstates
        ret