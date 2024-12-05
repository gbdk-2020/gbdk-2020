.include "global.s"

.area _CODE

; de : source
; a  : count
; (hblank_copy_destination) : destination
_hblank_copy_vram::
        ld hl, #rIE
        ld c, (hl)
        ld (hl), #IEF_STAT
        ld hl, #rSTAT
        ld b, (hl)
        ld (hl), #STATF_MODE00
        push bc
        ld bc, #_hblank_copy_vram_tail
        push bc

; de : source
; a  : count
; (hblank_copy_destination) : destination
_hblank_cpy_vram::
        ld h, d
        ld l, e
        ld (stack_save), sp

        ld sp, #_hblank_copy_destination
        pop de

        ld sp, hl               ; sp = sour
        ld h, d
        ld l, e                 ; hl = dest

1$:
        ld (vram_cpy_counter), a

        pop de                  ; prefetch
        pop bc                  ; prefetch
        xor a
        ldh (rIF), a            ; reset pending interrupts
        ld a, e                 ; prepare the first byte

        halt
        nop                     ; in case the halt bug is triggered

        ld (hl+), a             ; copy two bytes
        ld a, d
        ld (hl+), a
        ld a, c
        ld (hl+), a             ; copy two bytes
        ld a, b
        ld (hl+), a
.rept 5
            pop de              ; copy ten bytes
            ld a, e
            ld (hl+), a
            ld a, d
            ld (hl+), a
.endm
        pop de                  ; copy two bytes
        ld a, e
        ld (hl+), a
        ld (hl), d
        inc hl

        ld a, (vram_cpy_counter)
        dec a
        jr nz, 1$

        ld sp, #stack_save
        pop hl
        ld sp, hl
        ret

_hblank_copy_vram_tail:
        pop bc
        ld a, b
        ldh (rSTAT), a
        ld a, c
        ldh (rIE), a
        xor a
        ldh (rIF), a            ; reset pending interrupts
        ret


.area _DATA

vram_cpy_counter:
    .ds 1
stack_save:
    .ds 2
_hblank_copy_destination::
    .ds 2