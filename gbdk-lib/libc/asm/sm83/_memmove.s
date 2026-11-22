;--------------------------------------------------------------------------
;  _memmove.s
;
;  Copyright (c) 2026, Toxa
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

	.area   _CODE

; void *memmove (void *dest, const void *src, size_t n);
_memmove::
        ldhl sp, #2
        ld a, (hl+)
        ld h, (hl)
        ld l, c
        ld c, a
        ld a, b
        ld b, h
        ld h, a

        push de
        ld a, d
        cp h
        jr c, 2$
        jr nz, 1$
        ld a, e
        cp l
        jr c, 2$
        jr z, 3$
1$:
        ld a, c
        add e
        ld e, a
        ld a, b
        adc d
        ld d, a
        dec de

        add hl, bc
        dec hl
        
        inc b
        inc c
        jr 4$
5$:        
        ld a, (hl-)
        ld (de), a        
        dec de
4$:
        dec c
        jr nz, 5$
        dec b
        jr nz, 5$
        jr 3$
2$:
        inc b
        inc c
        jr 6$
7$:
        ld a, (hl+)
        ld (de), a        
        inc de
6$:
        dec c
        jr nz, 7$
        dec b
        jr nz, 7$
3$:
        pop bc
        pop hl
        pop af
        jp  (hl)