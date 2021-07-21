.include "global.s"

.area _CODE

_set_vram_byte::
        ; de = addr, hl = &t
        ldhl    sp,#2
        ld      a,(hl+)
        ld      e, a
        ld      a,(hl+)
        ld      d, a

        WAIT_STAT

        ; Write tile
        ld      a,(hl)
        ld      (de),a
        ret

_set_win_tile_xy::
        ldh     a,(.LCDC)
        and     #LCDCF_WIN9C00
        jr      z,.is98
        jr      .is9c
_set_bkg_tile_xy::
        ldh     a,(.LCDC)
        and     #LCDCF_BG9C00
        jr      nz,.is9c
.is98:
        ld      d,#0x98         ; DE = origin
        jr      .set_tile_xy
.is9c:
        ld      d,#0x9C         ; DE = origin

.set_tile_xy:
        push    bc
        ldhl    sp,#4

        ld      a, (hl+)
        ld      b, a
        ld      a, (hl+)

        ld      e, d
        swap    a
        rlca
        ld      c, a
        and     #0x03
        add     e
        ld      d, a
        ld      a, #0xE0
        and     c
        add     b
        ld      e, a            ; dest DE = BASE + 0x20 * Y + X

        WAIT_STAT
        ld      a, (hl)
        ld      (de), a

        pop     bc
        ret
