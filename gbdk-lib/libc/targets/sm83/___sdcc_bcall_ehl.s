        .include        "global.s"

        .area _HOME

___sdcc_bcall_ehl::                     ; Performs a long call.
        ldh     a, (__current_bank)
        push    af                      ; Push the current bank onto the stack
        ld      a, e
        ldh     (__current_bank), a
        ld      (rROMB0), a             ; Perform the switch
        rst     0x20
        push    hl
        ldhl    sp, #3
        ld      h, (hl)
        ld      l, a
        ld      a, h
        ldh     (__current_bank), a
        ld      (rROMB0), a
        ld      a, l
        pop     hl
        add     sp, #2
        ret