        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .ez80

        .area   _INITIALIZED
__current_2bpp_palette::
        .ds     0x02

        .area   _INITIALIZER
        .dw     0b0011001000010000

        .area   _HOME

; void set_tile_2bpp_data(uint16_t start, uint16_t ntiles, const void *src, uint16_t palette) __z88dk_callee __preserves_regs(iyh,iyl);
_set_tile_2bpp_data::
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
        push iy
        ld iy, #-4
        add iy, sp
        ld sp, iy        
        push bc
        jr 2$
        
1$:
        ex (sp), hl

        ld ixh, #8
6$:
        ld c, (hl)
        inc hl
        ld b, (hl)
        inc hl

        ld ixl, #8
5$:
        xor a
        srl b
        rla
        srl c
        rla

        push de                 ; save palette

        inc a
        dec a
        jr nz, 10$
        ; zero
        ld a, e
        jr 13$
10$:
        dec a
        jr nz, 11$
        ; one
        ld a, e
        jr 14$
11$:
        dec a
        jr nz, 12$
        ; two
        ld a, d
        jr 13$
12$:
        ;three
        ld a, d
14$:
        rra
        rra
        rra
        rra
13$:
        and #0x0F

        rra
        rr 0 (iy)
        rra
        rr 1 (iy)
        rra
        rr 2 (iy)
        rra
        rr 3 (iy)

        pop de                  ; restore palette

        dec ixl
        jr nz, 5$

        ld a, 0 (iy)
        out (.VDP_DATA), a
        ld a, 1 (iy)
        out (.VDP_DATA), a
        ld a, 2 (iy)
        out (.VDP_DATA), a
        ld a, 3 (iy)
        out (.VDP_DATA), a

        dec ixh
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
