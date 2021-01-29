;--------------------------------------------------------------------------
;  div.s
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

.globl	__divsuchar
.globl	__modsuchar
.globl	__divuschar
.globl	__moduschar
.globl	__divschar
.globl	__modschar
.globl	__divsint
.globl	__modsint
.globl	__divuchar
.globl	__moduchar
.globl	__divuint
.globl	__moduint

__divsuchar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)
        ld      b,#0

        call      signexte

	ld	e,c
	ld	d,b

	ret

__modsuchar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)
        ld      b,#0

        jp    signexte

__divuschar:
        ld      hl,#2+1
        ld      d, h
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)

        ld      a,c             ; Sign extend
        rlca
        sbc     a
        ld      b,a

        call      .div16

	ld	e,c
	ld	d,b

	ret

__moduschar:
        ld      hl,#2+1
        ld      d, h
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      c,(hl)

        ld      a,c             ; Sign extend
        rlca
        sbc     a
        ld      b,a

        call    .div16

        ret

__divschar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      l,(hl)

        ld      c,l

        call    .div8

        ld      e,c
        ld      d,b

        ret

__modschar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      l,(hl)

        ld      c,l

        call    .div8

        ;; Already in DE

        ret

__divsint:
        ld      hl,#2+3
        add     hl,sp

        ld      d,(hl)
        dec     hl
        ld      e,(hl)
        dec     hl
        ld      a,(hl)
        dec     hl
        ld      l,(hl)
        ld      h,a

        ld      b,h
        ld      c,l

        call    .div16

        ld      e,c
        ld      d,b

        ret

__modsint:
        ld      hl,#2+3
        add     hl,sp

        ld      d,(hl)
        dec     hl
        ld      e,(hl)
        dec     hl
        ld      a,(hl)
        dec     hl
        ld      l,(hl)
        ld      h,a

        ld      b,h
        ld      c,l

        call    .div16

        ;; Already in DE

        ret

        ;; Unsigned
__divuchar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      l,(hl)

        ld      c,l
        call    .divu8

        ld      e,c
        ld      d,b

        ret

__moduchar:
        ld      hl,#2+1
        add     hl,sp

        ld      e,(hl)
        dec     hl
        ld      l,(hl)

        ld      c,l
        call    .divu8

        ;; Already in DE

        ret

__divuint:
        ld      hl,#2+3
        add     hl,sp

        ld      d,(hl)
        dec     hl
        ld      e,(hl)
        dec     hl
        ld      a,(hl)
        dec     hl
        ld      l,(hl)
        ld      h,a

        ld      b,h
        ld      c,l
        call    .divu16

        ld      e,c
        ld      d,b

        ret

__moduint:
        ld      hl,#2+3
        add     hl,sp

        ld      d,(hl)
        dec     hl
        ld      e,(hl)
        dec     hl
        ld      a,(hl)
        dec     hl
        ld      l,(hl)
        ld      h,a

        ld      b,h
        ld      c,l

        call    .divu16

        ;; Already in DE

        ret

.div8::
.mod8::
        ld      a,c             ; Sign extend
        rlca
        sbc     a
        ld      b,a
signexte:
        ld      a,e             ; Sign extend
        rlca
        sbc     a
        ld      d,a

        ; Fall through to .div16

        ;; 16-bit division
        ;;
        ;; Entry conditions
        ;;   BC = dividend
        ;;   DE = divisor
        ;;
        ;; Exit conditions
        ;;   BC = quotient
        ;;   DE = remainder
        ;;   If divisor is non-zero, carry=0
        ;;   If divisor is 0, carry=1 and both quotient and remainder are 0
        ;;
        ;; Register used: AF,BC,DE,HL
.div16::
.mod16::
        ;; Determine sign of quotient by xor-ing high bytes of dividend
        ;;  and divisor. Quotient is positive if signs are the same, negative
        ;;  if signs are different
        ;; Remainder has same sign as dividend
        ld      a,b             ; Get high byte of dividend
        push    af              ; Save as sign of remainder
        xor     d               ; Xor with high byte of divisor
        push    af              ; Save sign of quotient

        ;; Take absolute value of divisor
        bit     7,d
        jr      Z,.chkde        ; Jump if divisor is positive
        sub     a               ; Substract divisor from 0
        sub     e
        ld      e,a
        sbc     a               ; Propagate borrow (A=0xFF if borrow)
        sub     d
        ld      d,a
        ;; Take absolute value of dividend
.chkde:
        bit     7,b
        jr      Z,.dodiv        ; Jump if dividend is positive
        sub     a               ; Substract dividend from 0
        sub     c
        ld      c,a
        sbc     a               ; Propagate borrow (A=0xFF if borrow)
        sub     b
        ld      b,a
        ;; Divide absolute values
.dodiv:
        call    .divu16
        jr      C,.exit         ; Exit if divide by zero
        ;; Negate quotient if it is negative
        pop     af              ; recover sign of quotient
        and     #0x80
        jr      Z,.dorem        ; Jump if quotient is positive
        sub     a               ; Substract quotient from 0
        sub     c
        ld      c,a
        sbc     a               ; Propagate borrow (A=0xFF if borrow)
        sub     b
        ld      b,a
.dorem:
        ;; Negate remainder if it is negative
        pop     af              ; recover sign of remainder
        and     #0x80
        ret     Z               ; Return if remainder is positive
        sub     a               ; Substract remainder from 0
        sub     e
        ld      e,a
        sbc     a               ; Propagate remainder (A=0xFF if borrow)
        sub     d
        ld      d,a
        ret
.exit:
        pop     af
        pop     af
        ret

.divu8::
.modu8::
        ld      b,#0x00
        ld      d,b
        ; Fall through to divu16

.divu16::
.modu16::
        ;; Check for division by zero
        ld      a,e
        or      d
        jr      NZ,.divide      ; Branch if divisor is non-zero
        ld      bc,#0x00        ; Divide by zero error
        ld      d,b
        ld      e,c
        scf                     ; Set carry, invalid result
        ret
.divide:
        ld      l,c             ; L = low byte of dividend/quotient
        ld      h,b             ; H = high byte of dividend/quotient
        ld      bc,#0x00        ; BC = remainder
        or      a               ; Clear carry to start
        ld      a,#16           ; 16 bits in dividend
.dvloop:
        ;; Shift next bit of quotient into bit 0 of dividend
        ;; Shift next MSB of dividend into LSB of remainder
        ;; BC holds both dividend and quotient. While we shift a bit from
        ;;  MSB of dividend, we shift next bit of quotient in from carry
        ;; HL holds remainder
        ;; Do a 32-bit left shift, shifting carry to L, L to H,
        ;;  H to C, C to B
        push    af              ; save number of bits remaining
        rl      l               ; Carry (next bit of quotient) to bit 0
        rl      h               ; Shift remaining bytes
        rl      c
        rl      b               ; Clears carry since BC was 0
        ;; If remainder is >= divisor, next bit of quotient is 1. This
        ;;  bit goes to carry
        push    bc              ; Save current remainder
        ld      a,c             ; Substract divisor from remainder
        sbc     e
        ld      c,a
        ld      a,b
        sbc     d
        ld      b,a
        ccf                     ; Complement borrow so 1 indicates a
                                ;  successful substraction (this is the
                                ;  next bit of quotient)
        jr      C,.drop         ; Jump if remainder is >= dividend
        pop     bc              ; Otherwise, restore remainder
        pop     af              ; recover # bits remaining, carry flag destroyed
        dec     a
        or      a               ; restore (clear) the carry flag
        jr      NZ,.dvloop
        jr      .nodrop
.drop:
        inc     sp
        inc     sp
        pop     af              ; recover # bits remaining, carry flag destroyed
        dec     a
        scf                     ; restore (set) the carry flag
        jr      NZ,.dvloop
        jr      .nodrop
.nodrop:
        ;; Shift last carry bit into quotient
        ld      d,b             ; DE = remainder
        ld      e,c
        rl      l               ; Carry to L
        ld      c,l             ; C = low byte of quotient
        rl      h
        ld      b,h             ; B = high byte of quotient
        or      a               ; Clear carry, valid result
        ret

