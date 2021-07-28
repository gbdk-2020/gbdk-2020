;--------------------------------------------------------------------------
;  mulchar.s
;
;  Copyright (c) 2017, Philipp Klaus Krause
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

; unsigned char x unsigned char multiplication is done by code generation.

.globl	__mulsuchar
.globl	__muluschar
.globl	__mulschar

; operands have different sign

__mulsuchar:
        ld      hl,#2+1
        ld      b, h
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)
        jr      signexte

__muluschar:
        ld      hl,#2
        ld      b, h
        add     hl,sp

        ld      e,(hl)
        inc     hl
        ld      c,(hl)
        jr      signexte

__mulschar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)

        ;; Need to sign extend before going in.
        ld      a,c
        rla
        sbc     a,a
        ld      b,a
signexte:
        ld      a,e
        rla
        sbc     a,a
        ld      d,a

        jp      __mul16

