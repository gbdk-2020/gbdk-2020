;--------------------------------------------------------------------------
;  mul.s
;
;  Copyright (C) 2000, Michael Hope
;  Copyright (C) 2021-2022, Sebastian 'basxto' Riedel (sdcc@basxto.de)
;  Copyright (c) 2021, Philipp Klaus Krause
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

        .module mul
        .area   _CODE

.globl  __mulsuchar
.globl  __muluschar
.globl  __mulschar
.globl  __muluchar
.globl  __mulint

; operands with different sign

__mulsuchar:
        ld      c, a
        jr      signexte

__muluschar:
        ld      c, e
        ld      e, a

signexte:
        ld      a,e
        rla
        sbc     a,a
        ld      d,a

        xor     a
        jr      .mul8

__mulschar:
        ; Sign-extend before going in.
        ld      c,a

        rla
        sbc     a,a
        ld      b,a

        ld      a,e
        rla
        sbc     a,a
        ld      d,a

__mulint:
        ;; 16-bit multiplication
        ;;
        ;; Entry conditions
        ;;   BC = multiplicand
        ;;   DE = multiplier
        ;;
        ;; Exit conditions
        ;;   BC = less significant word of product
        ;;
        ;; Register used: AF,BC,DE,HL
.mul16:
        ;; Let the smaller number loop
        ld      a,b
        cp      a,d
        jr      c, keep
        ;; d <= b
        ld      a, e
        ld      e, c
        ld      c, a
        ld      a, d
        ld      d, b
        ld      b, a
keep:
        ;; Optimise for the case when this side has 8 bits of data or
        ;; less.  This is often the case with support address calls.
        or      a
        jp      Z, .mul8
        
        ld      l,#0
        ld      b,#16
loop16:
        ;; Taken from z88dk, which originally borrowed from the
        ;; Spectrum rom.
        add     hl,hl
        rl      c
        rla                     ;DLE 27/11/98
        jr      NC,skip16
        add     hl,de
skip16:
        dec     b
        jr      NZ,loop16

        ;; Return in bc
        ld      c,l
        ld      b,h

        ret

__muluchar:
        ld      c, a
        xor     a
        ;; Clear the top
        ld      d, a

        ;; Version that uses an 8bit multiplicand
        ;;
        ;; Entry conditions
        ;;   C  = multiplicand
        ;;   DE = multiplier
        ;;   A  = 0
        ;;
        ;; Exit conditions
        ;;   BC = less significant word of product
        ;;
        ;; Register used: AF,BC,DE,HL
.mul8:
        ld      l,a
        ld      b,#8
        ld      a,c
loop8:
        add     hl,hl
        rla
        jr      NC,skip8
        add     hl,de
skip8:
        dec     b
        jr      NZ,loop8

        ;; Return in bc
        ld      c,l
        ld      b,h

        ret

