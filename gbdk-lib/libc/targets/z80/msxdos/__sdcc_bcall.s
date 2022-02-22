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
        pop af

        push de
        ld (__current_bank), a
        ld d, #>__banks_remap_table
        ld e, a
        ld a, (de)
        call .mapper_page_set
        pop de
        ret
