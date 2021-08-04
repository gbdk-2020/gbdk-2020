        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_bkg_data(uint8_t start, uint8_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
_set_bkg_data::
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
        
        ld a, c
        ld c, #.VDP_DATA
        inc a        
        jr 2$
            
1$:
        ld b, #32
3$:        
        outi
        jr nz, 3$
2$:
        dec a
        jr  nz, 1$
        
        ld h, d
        ld l, e
        jp (hl)
