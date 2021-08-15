;--------------------------------------------------------------------------
;  divsigned.s
;
;  Copyright (C) 2000-2010, Michael Hope, Philipp Klaus Krause
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

.area	_CODE

.globl	__divsint
.globl	__divschar

__divsint:
        pop     af
        pop     hl
        pop     de
        push    de
        push    hl
        push    af

        jp      __div16

__divschar:
        ld      hl, #2+1
        add     hl, sp

        ld      e, (hl)
        dec     hl
        ld      l, (hl)

__div8::
        ld      a, l            ; Sign extend
        rlca
        sbc     a,a
        ld      h, a
__div_signexte::
        ld      a, e            ; Sign extend
        rlca
        sbc     a,a
        ld      d, a
        ; Fall through to __div16

        ;; signed 16-bit division
        ;;
        ;; Entry conditions
        ;;   HL = dividend
        ;;   DE = divisor
        ;;
        ;; Exit conditions
        ;;   HL = quotient
        ;;   DE = remainder
        ;;
        ;; Register used: AF,B,DE,HL
__div16::
        ;; Determine sign of quotient by xor-ing high bytes of dividend
        ;;  and divisor. Quotient is positive if signs are the same, negative
        ;;  if signs are different
        ;; Remainder has same sign as dividend
        ld      a, h            ; Get high byte of dividend
        xor     a, d            ; Xor with high byte of divisor
        rla                     ; Sign of quotient goes into the carry
        ld      a, h            ; Get high byte of dividend
        push    af              ; Save sign of both quotient and reminder

        ; Take absolute value of dividend
        rla
        jr      NC, .chkde      ; Jump if dividend is positive
        sub     a, a            ; Substract dividend from 0
        sub     a, l
        ld      l, a
        sbc     a, a            ; Propagate borrow (A=0xFF if borrow)
        sub     a, h
        ld      h, a

        ; Take absolute value of divisor
.chkde:
        bit     7, d
        jr      Z, .dodiv       ; Jump if divisor is positive
        sub     a, a            ; Subtract divisor from 0
        sub     a, e
        ld      e, a
        sbc     a, a            ; Propagate borrow (A=0xFF if borrow)
        sub     a, d
        ld      d, a

        ; Divide absolute values
.dodiv:
        call    __divu16

.fix_quotient:
        ; Negate quotient if it is negative
        pop     af              ; recover sign of quotient
        ret	NC		; Jump if quotient is positive
        ld      b, a
        sub     a, a            ; Subtract quotient from 0
        sub     a, l
        ld      l, a
        sbc     a, a            ; Propagate borrow (A=0xFF if borrow)
        sub     a, h
        ld      h, a
        ld      a, b
	ret

__get_remainder::
        ; Negate remainder if it is negative and move it into hl
        rla
	ex	de, hl
        ret     NC              ; Return if remainder is positive
        sub     a, a            ; Subtract remainder from 0
        sub     a, l
        ld      l, a
        sbc     a, a             ; Propagate remainder (A=0xFF if borrow)
        sub     a, h
        ld      h, a
        ret

