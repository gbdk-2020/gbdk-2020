        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils
        .area   _HOME

; void set_tile_2bpp_data(uint16_t start, uint16_t ntiles, const void *src) __z88dk_callee __preserves_regs(iyh,iyl);
_set_tile_2bpp_data::
        pop de          ; pop ret address
        pop hl
                
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        
        ld bc, #.VDP_VRAM
        add hl, bc
                
        ld a, #0x01
        ld (__shadow_OAM_OFF), a        ; switch OFF copy shadow SAT

        SMS_WRITE_VDP_CMD h, l

        pop bc
        pop hl
        push de
        
        ld e, c
        ld d, b        
        
        ld c, #.VDP_DATA
        inc d
        inc e                
        xor a
        jr 2$
            
1$:
        ld b, #16
3$:        
        outi
        jr 4$
4$:
        outi
        jr 5$
5$:
        nop
        out (c), a
        jr 6$
6$:
        nop
        nop
        out (c), a
        nop
        jr nz, 3$
2$:
        dec e
        jr  nz, 1$

        dec d
        jr  nz, 1$

        xor a
        ld (__shadow_OAM_OFF), a        ; switch ON copy shadow SAT

        ret
