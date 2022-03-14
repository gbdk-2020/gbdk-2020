        .include        "global.s"

        .title  "gets"
        .module gets

        .area   _CODE

_gets::
        pop hl
        pop de
        push de
        push hl
        push de
        ld c, #63               ; limit input to 64 bytes including trailing zero
                                ; that match with internal buffer in scanf()
1$:
        push bc
        push de
        CALL_BDOS #_CONIN
        pop de
        pop bc
	cp #0x0d                ; CR
        jp z, 2$
        cp #0x05                ; EOF
        jp z, 2$
        ld (de), a
        inc de
        dec c
        jp nz, 1$
2$:
        xor a
        ld (de), a
        pop hl
        ret