        .include "global.s"

        .area   _CODE

        .globl ___sdcc_bcall_ehl
;
; default trampoline to call banked functions
; Usage:
;  ld   e, #<function_bank>
;  ld   hl, #<function>
;  call ___sdcc_bcall_ehl
;
___sdcc_bcall_ehl::
        push hl
        ld h, #>__banks_remap_table
        ld l, e                 ; e == new bank

        ld a, (__current_bank)
        ld l, (hl)              ; l == segment
        ld h, a                 ; h == old bank

        ld a, e
        ld (__current_bank), a  ; new bank to __current_bank 

        ld a, l                 ; set segment                 
        call .mapper_page_set
        ex (sp), hl             ; old bank is left on stack
        inc sp

        CALL_HL
        dec sp

        ex (sp), hl
        ld c, a
        ld a, h
        ld (__current_bank), a
        ld h, #>__banks_remap_table
        ld l, a
        ld a, (hl)
        call .mapper_page_set
        ld a, c
        pop hl
        ret
