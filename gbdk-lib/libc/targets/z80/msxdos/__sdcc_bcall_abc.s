        .include "global.s"

        .area   _CODE

        .globl ___sdcc_bcall
        .globl ___sdcc_bcall_abc

;
; trampoline to call banked functions
; used when legacy banking is enabled only
; Usage:
;   call ___sdcc_bcall
;   .dw  <function>
;   .dw  <function_bank>
;
___sdcc_bcall::
        ex (sp), hl
        ld c, (hl)
        inc hl
        ld b, (hl)
        inc hl
        ld a, (hl)
        inc hl
        inc  hl
        ex (sp), hl
;
; trampoline to call banked functions with __z88dk_fastcall calling convention
; Usage:
;  ld   a, #<function_bank>
;  ld   bc, #<function>
;  call ___sdcc_bcall_abc
;
___sdcc_bcall_abc::
        push hl
        ld l, a
        ld a, (__current_bank)
        ld h, #>__banks_remap_table
        ld l, (hl)
        ld h, a
        call .mapper_page_set
        ex (sp), hl

        inc sp
        call ___sdcc_bjump_abc
        dec sp

        ex (sp), hl
        ld a, h
        ld (__current_bank), a
        ld h, #>__banks_remap_table
        ld l, a
        ld a, (hl)
        call .mapper_page_set
        pop hl
        ret
;
___sdcc_bjump_abc:
        push bc
        ret
