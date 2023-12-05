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
        ld b, h
        
        ld hl, (rle_cursor)

        ld a, l
        or h
        ret z

        ld a, (rle_counter)
        or a
        ld c, a
        jr z, 1$

        ld a, (rle_current)
        bit 7, c
        jr nz, 7$
        jp 8$
1$:
        ;; Fetch the run
        ld c, (hl)
        inc hl

        ;; Negative means a run
        bit 7, c
        jr z, 2$
        ;; Expanding a run
        ld a, (hl)
        inc hl
3$:
        ld (de), a
        inc de

        dec b
        jr z, 6$
7$:
        inc c
        jp NZ, 3$
        jp 1$
2$:
        ;; Zero means end of a block
        inc c
        dec c
        jr z, 4$
        ;; Expanding a block
5$:
        ldi
        inc bc

        dec b
        jr z, 6$
8$:
        dec c
        jp NZ, 5$
        jp 1$
4$:
        ;; save state and exit
        ld hl, #0
        ld (rle_cursor), hl
        ld (rle_counter), hl
        ret
6$:
        ;; save state and exit
        ld (rle_cursor), hl
        ld hl, #rle_counter
        ld (hl), c
        inc hl
        ld (hl), a
        ld l, #1
        ret