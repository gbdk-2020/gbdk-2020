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
        ld hl, #rle_cursor
        ld (hl), e
        inc hl
        ld (hl), d
        inc hl
        xor a
        ld (hl+), a
        ld (hl), a
        inc a
        ret

_rle_decompress::
        ld b, a         ; b == count

        ld hl, #rle_cursor
        ld a, (hl+)
        ld h, (hl)
        ld l, a         ; hl == cursor

        or h
        ret z           ; return 0

        ld a, (rle_counter)
        or a
        ld c, a
        jr z, 1$

        ld a, (rle_current)
        bit 7, c
        jr nz, 10$
        jr 11$
1$:
        ;; Fetch the run
        ld c, (hl)
        inc hl
        ;; Negative means a run
8$:
        bit 7, c
        jr z, 2$
        ;; Expanding a run
        ld a, (hl+)
3$:
        ld (de), a
        inc de

        dec b
        jr z, 6$
10$:
        inc c
        jr nz, 3$
        jr 1$
2$:
        ;; Zero means end of a block
        inc c
        dec c
        jr z, 4$
        ;; Expanding a block
5$:
        ld a, (hl+)
        ld (de), a
        inc de

        dec b
        jr z, 6$
11$:
        dec c
        jr nz, 5$
        jr 1$
4$:
        ;; save state and exit
        ld hl, #rle_cursor
        xor a
        ld (hl+), a
        ld (hl), a
        ret             ; return 0
6$:
        ;; save state and exit
        ld d, h
        ld e, l
        ld hl, #rle_current
        ld (hl-), a
        ld a, c
        ld (hl-), a
        ld a, d
        ld (hl-), a
        ld (hl), e
        ld a, #1
        ret             ; return 1