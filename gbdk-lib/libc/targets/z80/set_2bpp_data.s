        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_bkg_2bpp_data(uint8_t start, uint8_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
_set_bkg_2bpp_data::
        pop de          ; pop ret address
        pop hl
        
        ld a, h
        ld h, #0
        
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        
        ld bc, #.VDP_VRAM
        add hl, bc
                
        set 6, h
       
        ld c, a
       
        SMS_WRITE_VDP_CMD h, l
        
        pop hl
        push de
        
        ld a, c
        inc a        
        ld c, #.VDP_DATA
        ld e, #0
        jr 2$
            
1$:
        ld d, #8
3$:        
        outi
        jr 4$
4$:
        outi
        jr 5$
5$:
        nop
        out (c), e
        jr 6$
6$:
        nop
        nop
        out (c), e
        dec d
        jr nz, 3$
2$:
        dec a
        jr  nz, 1$
        
        ret
