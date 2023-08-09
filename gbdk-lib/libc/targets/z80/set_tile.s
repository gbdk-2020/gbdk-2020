        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .globl  .coords_to_address

        .area   _HOME

; void set_vram_byte(uint8_t * addr, uint8_t v) __z88dk_callee __preserves_regs(iyh, iyl);
_set_vram_byte::
        pop de
        pop hl
        DISABLE_VBLANK_COPY
        WRITE_VDP_CMD_HL
        ex de, hl
        dec sp
        ex (sp), hl
        ld a, h
        out (.VDP_DATA), a
        ENABLE_VBLANK_COPY
        ret

; uint8_t * set_attributed_tile_xy(uint8_t x, uint8_t y, uint16_t t) __z88dk_callee __preserves_regs(iyh, iyl);
_set_attributed_tile_xy::
        pop hl          ; HL = ret
        pop de          ; DE = YX
        ex (sp), hl     ; HL = data

        ld a, d

        ld bc, #.VDP_TILEMAP
        call .coords_to_address
        ex de, hl

        DISABLE_VBLANK_COPY
        WRITE_VDP_CMD_HL

        ld c, #.VDP_DATA
        out (c), e
        VDP_DELAY
        out (c), d

        ENABLE_VBLANK_COPY
        ret

; uint8_t * set_tile_xy(uint8_t x, uint8_t y, uint8_t t) __z88dk_callee __preserves_regs(iyh, iyl);
_set_tile_xy::
        pop hl          ; HL = ret
        pop de          ; DE = YX
        dec sp
        ex (sp), hl     ; HL = data

        ld a, d

        ld bc, #.VDP_TILEMAP
        call .coords_to_address
        ex de, hl

        ld a, (.vdp_shift)
        ADD_A_REG16 h, l

        DISABLE_VBLANK_COPY
        WRITE_VDP_CMD_HL

        ld a, d
        out (.VDP_DATA), a

        ENABLE_VBLANK_COPY
        ret

; uint8_t * set_attribute_xy(uint8_t x, uint8_t y, uint8_t a) __z88dk_callee __preserves_regs(iyh, iyl);
_set_attribute_xy::
        pop hl          ; HL = ret
        pop de          ; DE = YX
        dec sp
        ex (sp), hl     ; HL = data

        ld a, d

        ld bc, #.VDP_TILEMAP
        call .coords_to_address
        ex de, hl

        ld a, (.vdp_shift)
        ADD_A_REG16 h, l

        DISABLE_VBLANK_COPY
        WRITE_VDP_CMD_HL

        in a, (.VDP_DATA)   ; skip tile index
        ld a, d
        out (.VDP_DATA), a

        ENABLE_VBLANK_COPY
        ret
