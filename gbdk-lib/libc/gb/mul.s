;--------------------------------------------------------------------------
;  mul.s
;
;  Copyright (C) 2000, Michael Hope
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

        ;; Originally from GBDK by Pascal Felber.

        .area   _CODE

.globl	__mulsuchar
.globl	__muluschar
.globl	__mulschar
.globl	__muluchar
.globl	__mulint

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
        ld      hl,#2
        add     hl,sp

        ld      e,(hl)
        inc     hl
        ld      l,(hl)

        ;; Need to sign extend before going in.
        ld      c,l

        ld      a,l
        rla
        sbc     a,a
        ld      b,a
signexte:
        ld      a,e
        rla
        sbc     a,a
        ld      d,a

        jp      .mul16

__muluchar:
        ld      hl,#2
        add     hl,sp

        ld      e,(hl)

        inc     hl
        ld      c,(hl)

        ;; Clear the top
        xor     a
        ld      d,a
        ld      b,a

        jp      .mul16

__mulint:
        ld      hl,#2
        add     hl,sp

        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a

        ;; Parameters:
        ;;      HL, DE (left, right irrelivent)
        ld      b,h
        ld      c,l

        ;; 16-bit multiplication
        ;;
        ;; Entry conditions
        ;;   BC = multiplicand
        ;;   DE = multiplier
        ;;
        ;; Exit conditions
        ;;   DE = less significant word of product
        ;;
        ;; Register used: AF,BC,DE,HL
.mul16:
        ld      hl,#0
        ld      a,b
        ; ld c,c
        ld      b,#16

        ;; Optimise for the case when this side has 8 bits of data or
        ;; less.  This is often the case with support address calls.
        or      a
        jp      NZ,1$

        ld      b,#8
        ld      a,c
1$:
        ;; Taken from z88dk, which originally borrowed from the
        ;; Spectrum rom.
        add     hl,hl
        rl      c
        rla                     ;DLE 27/11/98
        jp      NC,2$
        add     hl,de
2$:
        dec     b
        jr      NZ,1$

        ;; Return in DE
        ld      e,l
        ld      d,h

        ret

