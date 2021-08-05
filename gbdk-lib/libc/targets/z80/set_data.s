        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_sprite_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
; void set_bkg_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
_set_sprite_data::
_set_bkg_data::
        pop de          ; pop ret address
        pop hl
                
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        
        ld bc, #.VDP_VRAM
        add hl, bc
                
        set 6, h
       
        SMS_WRITE_VDP_CMD h, l

        pop bc        
        pop hl
        push de
                
        ld e, c
        ld d, b
        
        ld c, #.VDP_DATA
        inc d
        inc e        
        jr 2$
            
1$:
        ld b, #32
3$:        
        outi
        jr nz, 3$
2$:
        dec e
        jr  nz, 1$

        dec d
        jr nz, 1$
        
        ret
