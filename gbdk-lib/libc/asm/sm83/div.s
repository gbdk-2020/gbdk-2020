;--------------------------------------------------------------------------
;  div.s
;
;  Copyright (C) 2000, Michael Hope
;  Copyright (C) 2021, Sebastian 'basxto' Riedel (sdcc@basxto.de)
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
	;; Updated by Phidias618
        .module divmod
        .area   _CODE

.globl  __divuchar
.globl  __moduchar
.globl  __divuint
.globl  __moduint

.globl  __divsuchar
.globl  __modsuchar
.globl  __divuschar
.globl  __moduschar
.globl  __divschar
.globl  __modschar

.globl  __divsint
.globl  __modsint


; unsigned division

__moduchar:
        ld	c, e
        ld      e, a
        xor a
        ld b, a
        ld d, a
__moduint:
        call    .divmod_uint_bcde

        ld      c, e
        ld      b, d

        ret

.divmod_uchar_bcde:
__divuchar:
        ld		c, e
        ld      e, a
        xor a
        ld b, a
        ld d, a
        ; Fall through .divmod_uint_bcde
.divmod_uint_bcde:
__divuint::
        ; computes the quotient and the remainder of X / Y
        ; X is stored in de
        ; Y is stored in bc
        ; outputs the quotient in bc
        ; outputs the remainder in de
        ; if Y = 0 then the carry is set and quotient = 0 and remainder = 0
        ; otherwise the carry is cleared

        
        ; stores -Y in hl and check that -Y is not zero
        dec bc
        ld a, b
        cpl
        ld h, a
        ld a, c
        cpl
        ld l, a
        
        OR h
        jr z, .division_by_zero
        
        ; if X < Y then
        ;	return quotient = 0 and remainder = X
        ; else
        ;	computes the largest (Y << K) such that (Y << K) <= X
        ; 	put the 17 bit value -(Y << (K + 1)) into hl with the MSB stored using the carry
        ; 	put K, using a base 1 representation, using the most significant bits of bc
        ld bc, #0
        
        ld a, e
        add l
        ld a, d
        adc h
        jr c, 10$
        ; X < Y
        ; bc already stores 0
        ret
0$:
        rr b
        rr c
10$:	
        add hl, hl
        jr nc, 1$
        
        ld a, e
        add l
        ld a, d
        adc h
        jr c, 0$
        scf
1$:

        ; swaps the content of hl and de
        ld a, h
        ld h, d
        ld d, a
        
        ld a, l
        ld l, e
        ld e, a
        
        ; computes X / Y one bit at a time using the following algorithm
        ; r = X
        ; q  = 0
        ; while K >= 0
        ;	b = 0
        ;	if (Y << K) <= r then
        ;		r += -(Y << K)
        ;		b = 1
        ;	q = (q << 1) | b
        ;	K -= 1
        ;
        ; 
        ; r is stored in hl
        ; q is stored in bc
        ; storing both q and K in bc will not create issue as they will not use the same bits at the same time
        ; -(Y << K) is stored in de
2$:
        ; on the first iteration :
        ;	this shifts DE such that DE now stores -(Y << K)
        ; on every iteration but the first:
        ;	transforms -(Y << K) into -(Y << (K-1))
        ;	sra d is not used as the MSB should always be filled with a 1 when shifting de
        ;	this works because the carry will always be set if this is not the first iteration
        rr  d
        rr  e
        
        ; compare remainder with (Y << K)
        ld a, e 
        add l
        ld a, d
        adc h
        
        jr nc, 3$
        add hl, de
3$:
        ; fill bc with one bit of the result and decrements K at the same time
        rl c
        rl b
        
        jr c, 2$
        
        ld d, h
        ld e, l
        ; status of the registers
        ; bc = quotient
        ; de = remainder
        ; carry = 0
        
        ret
.division_by_zero:
        ; returns both a quotient of 0 and a remainder of 0
        ; bc should always store 0xFFFF by that point
        inc bc
        ld d, b
        ld e, c
        scf					; sets the carry in order to indicate that a division by zero occured
        ret


; mixed sign division

__modsuchar:
        call .divmod_suchar_bcde
        ld b, d
        ld c, e
        ret
.divmod_suchar_bcde:
__divsuchar:
        ld d, #0
        ld c, e
        ld e, a
        
        jr signext_c

__moduschar:
        call .divmod_uschar_bcde
        ld b, d
        ld c, e
        ret
.divmod_uschar_bcde:
__divuschar:
        ld b, #0
        ld c, e
        ld e, a
		add a
        sbc a
        ld d, a
        
        jr .divmod_sint_bcde
		
; signed division

__modschar:
        call .divmod_schar_bcde
        ld b, d
        ld c, e
        ret
__modsint::
        call .divmod_sint_bcde
        ld b, d
        ld c, e
        ret

; these 2 functions exists for compatibility reasons
.div8::
.mod8::
        ld a, c
        ; Fall through .divmod_schar_bcde
.divmod_schar_bcde:
__divschar:
        ld		c, e
        ld      e, a
		add a
        sbc a
        ld d, a
signext_c:
        ld a, c
        rlca
        sbc a
        ld b, a
        ; Fall through .divmod_sint_bcde
.divmod_sint_bcde:
__divsint::
        ; saves the sign of the quotient as the carry and the sign of the remainder as the 7th bit of A
        ld a, d		; high byte of dividend
        xor c		; high byte of divisor
        rla			; save the 7th bit of A as the carry
        ld a, d
        push af
        
        ; take the absolute value of de
        add a
        jr nc, 0$
        
        xor a
        sub e
        ld e, a
        sbc a
        sub d
        ld d, a
0$:
        ; take the absolute value of bc
        bit 7, b
        jr z, 1$
        
        xor a
        sub c
        ld c, a
        sbc a
        sub b
        ld b, a
1$:
        call .divmod_uint_bcde
        jr c, .error
        pop af		; retrieve the signs the the quotient and the sign of the remainder
        
        jr nc, 2$
        ld l, a		; saves the sign of the remainder
        ; negates the quotient
        xor a
        sub c
        ld c, a
        sbc a
        sub b
        ld b, a
        
        ld a, l
2$:
        rlca
        ret nc
        ; negates the remainder
        xor a
        sub e
        ld e, a
        sbc a
        sub d
        ld d, a
        
        or a			; clears the carry
        ret
.error:
        pop hl			; do not pop af in order to preserve the carry as set
        ret


;	the following functions are here in order to not break compatibility

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
	ld hl, #.divmod_sint_bcde
swap_bc_de_jp_hl:
        ld a, b
        ld b, d
        ld d, a
        
        ld a, c
        ld c, e
        ld e, a
        
        jp (hl)
.divu8::
.modu8::
        ld      b,#0x00
        ld      d,b
        ; Fall through to .divu16
.divu16::
.modu16::
        ld hl, #.divmod_uint_bcde
        jr swap_bc_de_jp_hl
