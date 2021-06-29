.include "global.s"

.area _CODE

_get_vram_byte::
        ldhl    sp,#2
        ld      a,(hl+)
        ld      h,(hl)
        ld      l,a
        WAIT_STAT
        ld      e,(hl)
        ret

_get_win_tile_xy::
        ldh     a,(.LCDC)
        bit     6,a
        jr      z,.is98
        jr      .is9c
_get_bkg_tile_xy::
        ldh     a,(.LCDC)
        bit     3,a
        jr      nz,.is9c
.is98:
        ld      d,#0x98         ; DE = origin
        jr      .get_tile_xy
.is9c:
        ld      d,#0x9C         ; DE = origin

.get_tile_xy:
        ldhl    sp,#3

        ld      a, (hl-)
        ld      l, (hl)

        ld      e, d
        swap    a
        rlca
        ld      h, a
        and     #0x03
        add     e
        ld      d, a
        ld      a, #0xE0
        and     h
        add     l
        ld      l, a
        ld      h, d            ; dest DE = BASE + 0x20 * Y + X

        WAIT_STAT
        ld      e, (hl)

        ret
