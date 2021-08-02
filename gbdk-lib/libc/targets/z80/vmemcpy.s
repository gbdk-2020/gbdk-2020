        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void vmemcpy (unsigned int dst, const void *src, unsigned int size) __z88dk_callee __preserves_regs(iyh,iyl);
_vmemcpy::
        pop de          ; pop ret address
        pop hl          ; dst
        set 6, h
        SMS_WRITE_VDP_CMD
        
        pop hl          ; src
        pop bc          ; size
        
        dec bc
        
        ld a, b         ; HI(size)
        ld b, c         ; LO(size)
        
        inc a
        inc b
        
        ld c, #.VDP_DATA
1$:
        outi
        jp  nz, 1$      ; 10 = 26 (VRAM safe)

        dec a
        jp  nz, 1$
        
        ld h, d
        ld l, e
        jp (hl)
