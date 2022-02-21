        .include        "global.s"

        .title  "Runtime"
        .module Runtime

        .ez80

        ;; Ordering of segments for the linker.
        .area   _CODE
        .area   _CODE_0
        .area   _HOME
        .area   _BASE
        .area   _LIT
        .area   _GSINIT
        .area   _GSFINAL
        .area   _INITIALIZER

        .area   _DATA
        .area   _INITIALIZED
        .area   _BSEG
        .area   _BSS
        .area   _HEAP
        .area   _HEAP_END

        ; program startup code
        .area   _CODE
.start::
        xor a
        ld bc, #l__DATA
        ld hl, #s__DATA
        call .memset_simple     ; initialize veriables in RAM with zero

        call .gsinit

        ld a, (___overlay_count)
        and a
        call nz, .load_overlays 
        jp c, .exit_error

        ld hl, #.LS_FILE_BUFFER
        xor a
        ld b, a
        ld c, (hl)
        or c
        jr z, 1$
        
        add hl, bc
        inc hl
        xor a
        ld (hl), a
        ld c, a                 ; param counter

        ld hl, #(.LS_FILE_BUFFER + 1)
3$:
        ld a, (hl)
        or a
        jr z, 6$
        cp #0x20
        jr nz, 6$
        inc hl
        jr 3$
6$:
        jr z, 2$

        inc c
        push hl
4$:
        inc hl
        ld a, (hl)
        or a
        jr z, 2$
        cp #0x20
        jr nz, 4$
5$:
        ld (hl), #0
        inc hl
        jr 3$
2$:
        ld hl, #0
        add hl, sp
        push hl                 ; push pointer to argv array on stack
1$:
        ld b, #0
        push bc                 ; push argc
                                ; reverse argv order
        ld e, c
        srl e
        jr z, 7$                ; only 1 parameter

        dec c
        ld ixh, b
        ld ixl, c
        add ix, ix
        ld b, h
        ld c, l
        add ix, bc
8$: 
        ld d, (hl)
        ld a, 0 (ix)
        ld 0 (ix), d
        ld (hl), a
        inc hl
        ld d, (hl)
        ld a, 1 (ix)
        ld 1 (ix), d
        ld (hl), a
        inc hl
        dec ix
        dec ix
        dec e
        jr nz, 8$
7$:
        call _main
.exit:
        ld b, l
        CALL_BDOS #_TERM        ; try terminate usind MSX DOS 2 function
        JP_BDOS #_TERM0
_exit::
        pop hl
        pop hl
        jr .exit

.exit_error:
        ld hl, #1$
        call _puts
        JP_BDOS #_TERM0
1$:
        .asciz "ERROR LOADING!\r\n"

        .area   _GSINIT
.gsinit::
        ;; initialize static storage variables
        ld bc, #l__INITIALIZER
        ld hl, #s__INITIALIZER
        ld de, #s__INITIALIZED
        call .memcpy_simple

        .area   _GSFINAL
        ret

        .area   _CODE
        ;; fills memory at HL of length BC with A, clobbers DE
.memset_simple::
        ld e, a        
        ld a, c
        or b
        ret z
        ld (hl), e
        dec bc
        ld d, h
        ld e, l
        inc de

        ;; copies BC bytes from HL into DE
.memcpy_simple::
        ld a, c
        or b
        ret z
        ldir
        ret

        ;; load overlays, count in A
.load_overlays:
        ld c, a
        ld b, #1
1$:
        push bc
        call .load_overlay
        pop bc
        ret c                  ; error
        ld a, b
        cp c
        jr z, 2$
        inc b
        jr 1$
2$:
        or a                    ; clear carry
        ret

.load_overlay:
        or a
        ret

___overlay_count::
        .db 0
.overlay_fcb:
        .db 0                   ; drive
___overlay_name::
        .ascii "OVERLAYS"       ; filename
        .ascii "000"            ; extension
        .db 0                   ; extent
        .db 0                   ; attributes
.overlay_fcb_record_size:
        .db 0                   ; extent       ; msx-dos1: record size low byte
        .db 0                   ; record count ; msx-dos1: record size high byte
.overlay_fcb_file_size:
        .db 0, 0, 0, 0          ; file size
        .db 0, 0, 0, 0          ; volume id
        .ds 8                   ; internal
        .db 0                   ; current record within extent
.overlay_fcb_random_pos:
        .db 0, 0, 0, 0          ; Random record number
