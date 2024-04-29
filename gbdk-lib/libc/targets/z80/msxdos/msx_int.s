        .include        "global.s"

        .title  "INT Handler"
        .module INTHandler

        .globl  .sys_time, .vbl_done
        .globl  .OUTI128, __shadow_OAM_base

        .area   _GSINIT

        ld a, #0xc3
        di
        ld (.LS_INT_VECTOR), a
        ld hl, #_INT_ISR
        ei
        ld (.LS_INT_VECTOR + 1), hl

        .area   _HOME

_INT_ISR::
        push af
        push bc
        push de
        push hl
        push iy
        push ix

        in a, (.VDP_STAT)
        and #.STATF_INT_VBL
        jp z, 2$
        ;; handle VBlank
        
        ld hl, (.sys_time)
        inc hl
        ld (.sys_time), hl

        ld a, #1
        ld (.vbl_done), a

        ;; transfer shadow OAM
        ld a, (__shadow_OAM_OFF)        ; check transfer is OFF
        or a
        jp nz, 1$
        ld hl, #__shadow_OAM_base
        ld h, (hl)
        ld l, a                         ; a == 0 here
        or h
        jp z, 1$

        ld c, #.VDP_CMD
        ld a, #<.VDP_SAT
        out (c), a
        ld a, #>.VDP_SAT
        out (c), a
        dec c                           ; c == .VDP_DATA
        call .OUTI128
1$:

        ;; call user-defined VBlank handlers
        ld hl, (.VBLANK_HANDLER0)
        ld a, h
        or l
        jp z, 2$
        CALL_HL

        ld hl, (.VBLANK_HANDLER1)
        ld a, h
        or l
        jp z, 2$
        CALL_HL

        ld hl, (.VBLANK_HANDLER2)
        ld a, h
        or l
        jp z, 2$
        CALL_HL

2$:             
        pop ix
        pop iy
        pop hl
        pop de
        pop bc
        pop af
        ei
        reti        
                   
; void add_VBL(int_handler h) __z88dk_fastcall __preserves_regs(d, e, iyh, iyl);
_add_VBL::
        ld b, h
        ld c, l

.add_VBL::
        ld hl, #.VBLANK_HANDLER0 

        ;; Add interrupt routine in BC to the interrupt list in HL
.add_int::
1$:
        ld a, (hl)
        inc hl
        or (hl)
        jr z, 2$
        inc hl
        jr 1$
2$:
        ld (hl), b
        dec hl
        ld (hl), c        
        ret

; void remove_VBL(int_handler h) __z88dk_fastcall __preserves_regs(iyh, iyl);
_remove_VBL::
        ld b, h
        ld c, l

        ;; Remove interrupt routine in BC from the VBL interrupt list
        ;; falldown to .remove_int
.remove_VBL::
        ld hl, #.VBLANK_HANDLER0

        ;; Remove interrupt BC from interrupt list HL if it exists
        ;; Abort if a 0000 is found (end of list)
.remove_int::
1$:
        ld e, (hl)
        inc hl
        ld d, (hl)
        inc hl
        ld a, e
        or d
        ret z           ; No interrupt found

        ld a, e
        cp c
        jr nz, 1$
        
        ld a, d
        cp b
        jr nz, 1$

        ld d, h
        ld e, l
        dec de
        dec de

        ;; Now do a memcpy from here until the end of the list
2$:
        ld a, (hl)
        ldi
        or (hl)
        ldi
        jr nz, 2$

_remove_LCD::
.remove_LCD::
_add_LCD::
.add_LCD::
_remove_TIM::
_remove_SIO::
_remove_JOY::
_add_TIM::
_add_SIO::
_add_JOY::
.empty_function:      
        ret
   
        .area   _INITIALIZED
        
.VBLANK_HANDLER0:
        .ds     0x02
.VBLANK_HANDLER1:
        .ds     0x02
.VBLANK_HANDLER2:
        .ds     0x02
        .ds     0x02
        
        .area   _INITIALIZER
        
        .dw     0x0000 
        .dw     0x0000 
        .dw     0x0000
        .dw     0x0000
