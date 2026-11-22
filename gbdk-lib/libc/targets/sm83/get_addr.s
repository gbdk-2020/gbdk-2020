.include "global.s"

.area _CODE

_get_win_xy_addr::
        and     #0x1f
        ld      d, a
        ldh     a, (rLCDC)
        and     #LCDCF_WIN9C00
        jr      z, .is98
        jr      .is9c

_get_bkg_xy_addr::
        and     #0x1f
        ld      d, a
        ldh     a, (rLCDC)
        and     #LCDCF_BG9C00
        jr      nz, .is9c
.is98:
        ld      b, #0x98        ; B = origin
        jr      .calculate_ofs
.is9c:
        ld      b, #0x9C        ; B = origin

.calculate_ofs:
        ld      a, e
        swap    a
        rlca
        ld      c, a
        and     #0x03
        or      b
        ld      b, a
        ld      a, #0xE0
        and     c
        or      d
        ld      c, a            ; BC = (B << 8) + 0x20 * Y + X

        ret
