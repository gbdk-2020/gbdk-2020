        .include "global.s"

        .title  "BGB_emu"
        .module BGB_emu

        .globl  _sprintf

        .area   _DATA

ret_save:
        .ds     0x02
printf_buffer:
        .ds     0x80

        .area   _HOME
    
        ;; BGB_printf(fmt, ...)
_BGB_printf::
        di
        pop de
        ld hl, #ret_save
        ld (hl), e
        inc hl
        ld (hl), d
        
        ld de, #printf_buffer
        push de
        call _sprintf
        pop hl

        ld d,d
        jr 1$
        .dw 0x6464
        .dw 0x0001
        .dw #printf_buffer
        .dw 0        
1$:
        ld hl, #ret_save
        ld a, (hl+)
        ld h, (hl)
        ld l, a
        ei
        jp (hl)
