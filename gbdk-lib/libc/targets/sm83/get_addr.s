.include "global.s"

.area _CODE

_get_win_xy_addr::
        ldh     a, (.LCDC)
        and     #LCDCF_WIN9C00
        jr      z, .is98
        jr      .is9c

_get_bkg_xy_addr::
        ldh     a, (.LCDC)
        and     #LCDCF_BG9C00
        jr      nz, .is9c
.is98:
        ld      d, #0x98        ; DE = origin
        jr      .calculate_ofs
.is9c:
        ld      d, #0x9C        ; DE = origin

.calculate_ofs:
        ldhl    sp, #3

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
        ld      e, a            ; dest DE = BASE + 0x20 * Y + X

        ret
