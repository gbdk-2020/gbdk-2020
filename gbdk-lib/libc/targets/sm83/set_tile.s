.include "global.s"

.area _CODE

_set_vram_byte::
        WAIT_STAT_HL

        ; Write tile
        ld      (de),a
        ret

_set_win_tile_xy::
        ld      d, a
        ldh     a, (.LCDC)
        and     #LCDCF_WIN9C00
        jr      z, .is98
        jr      .is9c

_set_bkg_tile_xy::
        ld      d, a
        ldh     a, (.LCDC)
        and     #LCDCF_BG9C00
        jr      nz, .is9c
.is98:
        ld      b, #0x98
        jr      .set_tile_xy
.is9c:
        ld      b,#0x9C

.set_tile_xy:                   ; DE = XY; B = origin
        ld      l, #0x1f
        ld      a, d
        and     l
        ld      d, a
        ld      a, e
        and     l

        ld      c, b
        swap    a
        rlca
        ld      e, a
        and     #0x03
        add     c
        ld      b, a
        ld      a, #0xE0
        and     e
        add     d
        ld      c, a            ; dest BC = (BASE << 8) + 0x20 * Y + X

        ldhl    sp, #2
        WAIT_STAT
        ld      a, (hl)
        ld      (bc), a         ; return BC as result

        pop     hl
        inc     sp
        jp      (hl)
