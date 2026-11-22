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
        ld a, e
        ld (hl+), a
        ld a, d
        ld (hl+), a
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
        jr z, 0$
        
        ld c, a
        add a
        ld a, (rle_current)
        jr c, 2$
        jr 5$
0$:
        ;; Fetch the run
        ld a, (hl+)
        ld c, a
        ;; Negative means a run
        add a
        jr nc, 3$
        ;; Expanding a run
        ld a, (hl+)
1$:
        ld (de), a
        inc de

        dec b
        jr z, 7$
2$:
        inc c
        jr nz, 1$
        jr 0$
3$:
        ;; Zero means end of a block
        jr z, 6$
        ;; Expanding a block
4$:
        ld a, (hl+)
        ld (de), a
        inc de

        dec b
        jr z, 7$
5$:
        dec c
        jr nz, 4$
        jr 0$
6$:
        ;; save state and exit
        ld hl, #rle_cursor
        xor a
        ld (hl+), a
        ld (hl), a
        ret             ; return 0
7$:
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
