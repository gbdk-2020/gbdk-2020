;--------------------------------------------------------------------------
;  reverse.s
;
;  Copyright (C) 2020, Tony Pavlov
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

        .module reverse

        .area   _HOME

_reverse::
        lda     HL, 2(SP)
        ld      A, (HL+)
        ld      H, (HL)
        ld      L, A            ; HL: s
        
        push    BC
        ld      B, H
        ld      C, L            ; BC: s
        
        ld      DE, #0
1$:     ld      A, (HL+)
        or      A
        jr      Z, 2$
        inc     DE
        jr      1$

2$:
        srl     D
        rr      E
        
        ld      A, E
        or      D
        jr      Z, 3$

        dec     HL
        dec     HL

        inc     D
        inc     E
        jr      5$
4$:
        ld      A, (HL)
        push    AF
        ld      A, (BC)
        ld      (HL-), A
        pop     AF
        ld      (BC), A
        inc     BC
5$:
        dec     E
        jr      NZ, 4$
        dec     D
        jr      NZ, 4$
        
3$:
        pop     BC
        lda     HL, 2(SP)
        ld      E, (HL)
        inc     HL
        ld      D, (HL)
        ret
