        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _HOME

; void set_tile_1bpp_data(uint16_t start, uint16_t ntiles, const void *src, uint16_t colors) __z88dk_callee __preserves_regs(iyh,iyl);
_set_tile_1bpp_data::
        DISABLE_VBLANK_COPY     ; switch OFF copy shadow SAT

        pop de                  ; pop ret address
        pop hl
                
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        
        ld bc, #.VDP_VRAM
        add hl, bc
                
        VDP_WRITE_CMD h, l

        ex de, hl               ; hl = ret

        pop bc                  ; bc = ntiles
        pop de                  ; de = src
        ex (sp), hl             ; hl = palette

        ex de, hl

        inc b
        inc c
        push ix

        ld ixh, d
        ld ixl, e               ; ix == palette

        push iy
        ld iy, #-4
        add iy, sp
        ld sp, iy        
        push bc
        jr 2$
        
1$:
        ex (sp), hl

        ld d, #8
6$:
        ld c, (hl)
        inc hl

        ld e, #8
5$:
        srl c

        jr c, 10$
        ld a, ixh
        jr 11$
10$:
        ld a, ixl
11$:
        rra
        rr 0 (iy)
        rra
        rr 1 (iy)
        rra
        rr 2 (iy)
        rra
        rr 3 (iy)

        dec e
        jr nz, 5$

        ld a, 0 (iy)
        out (.VDP_DATA), a
        ld a, 1 (iy)
        out (.VDP_DATA), a
        ld a, 2 (iy)
        out (.VDP_DATA), a
        ld a, 3 (iy)
        out (.VDP_DATA), a

        dec d
        jr nz, 6$
2$:
        ex (sp), hl

        dec l
        jr  nz, 1$

        dec h
        jr  nz, 1$

        ld iy, #6
        add iy, sp
        ld sp, iy
        pop iy
        pop ix

        ENABLE_VBLANK_COPY        ; switch ON copy shadow SAT

        ret
