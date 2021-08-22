        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void vmemcpy (unsigned int dst, const void *src, unsigned int size) __z88dk_callee __preserves_regs(iyh,iyl);
_set_data::
_vmemcpy::
        pop de          ; pop ret address
        pop hl          ; dst
        
        SMS_WRITE_VDP_CMD h, l
        
        pop hl          ; src
        pop bc          ; size
        
        ld a, b         ; HI(size)
        ld b, c         ; LO(size)
        
        ld c, #.VDP_DATA

        rlc b
        rrc b           ; check b is zero
        jr  z, 2$ 
1$:
        outi
        jp  nz, 1$      ; 10 = 26 (VRAM safe)
2$:
        inc a
        jp  4$          
3$:
        outi
        jp  nz, 3$      ; 10 = 26 (VRAM safe)
4$:
        dec a
        jp  nz, 3$
        
        ld h, d
        ld l, e
        jp (hl)
