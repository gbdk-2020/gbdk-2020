        .include "global.s"

        .module RLE_DECOMPRESS

        .area _DATA
 
rle_cursor:
        .ds 0x02
rle_counter:
        .ds 0x01
rle_current:
        .ds 0x01

        .area _CODE

_rle_init::
        ld (rle_cursor), hl
        xor a
        ld hl, #rle_counter
        ld (hl), a
        inc hl
        ld (hl), a
        ld l, #1
        ret

_rle_decompress::
        ;; Pop the return address
        pop hl
        pop de
        dec sp
        ex (sp), hl
        ld c, h
        
        ld hl, (rle_cursor)

        ld a, l
        or h
        ret z

        ld a, (rle_counter)
        or a
        ld b, a
        jr z, 0$

        add a
        ld a, (rle_current)
        jr c, 2$
        jp 5$
0$:
        ;; Fetch the run
        ld b, (hl)
        inc hl

        ;; Negative means a run
        ld a, b
        add a
        jr nc, 3$
        ;; Expanding a run
        ld a, (hl)
        inc hl
1$:
        ld (de), a
        inc de

        dec c
        jr z, 7$
2$:
        inc b
        jp NZ, 1$
        jp 0$
3$:
        ;; Zero means end of a block
        jr z, 6$
        ;; Expanding a block
4$:
        ldi
        inc bc

        dec c
        jr z, 7$
5$:
        djnz 4$
        jp 0$
6$:
        ;; save state and exit
        ld h, a                        ; h = 0
        ld l, a                        ; l = 0
        ld (rle_cursor), hl
        ld (rle_counter), hl
        ret
7$:
        ;; save state and exit
        ld (rle_cursor), hl
        ld hl, #rle_counter
        ld (hl), b
        inc hl
        ld (hl), a
        ld l, #1
        ret
