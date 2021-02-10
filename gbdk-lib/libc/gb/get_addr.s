.include "global.s"

.area _CODE

_get_win_map_addr::
        ldh     a,(.LCDC)
        bit     6,a
        jr      z,.is98
        jr      .is9c

_get_bkg_map_addr::
        ldh     a,(.LCDC)
        bit     3,a
        jr      nz,.is9c
.is98:
        ld      de,#0x9800      ; DE = origin
        ret
.is9c:
        ld      de,#0x9C00      ; DE = origin
        ret
