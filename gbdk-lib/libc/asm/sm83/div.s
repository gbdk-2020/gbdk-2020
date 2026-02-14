;--------------------------------------------------------------------------
;  div.s
;
;  Copyright (c) 2026, Phidias618
;
;--------------------------------------------------------------------------

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
        ld	c, e
        ld  e, a
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
		
	    ; stores -Y in hl
        xor a
		sub c
		ld l, a
		sbc a
		sub b
		ld h, a

		inc a
		jr z, small_y				; jump if 1 <= Y <= 256
		
		ld a, h
		or l
		jr z, division_by_zero

		; if this part of the code is reached then Y > 256
		; Thus the quotient will fit within 8 bits
		xor a
		ld b, a				; set the upper byte of the quotient to 0

		; computes the largest K such that ((-Y) << K) fits within 16 bits
		; ((-Y) << (K + 1)) will be stored in hl
		; K will be placed as a base 1 number using the most significant bits of C
0$:
		rra
		add hl, hl
		jr c, 0$
		ld c, a

		ld a, l
		ld l, e
		ld e, a

		ld a, d
		jr ld_dh_ha
small_y:
		jr c, division_by_256			; the algorithm below breaks if Y = 256
		; It would have been possible to check for 0 <= Y < 256 when jumping to small_y
		; However this would be slower by a few cycles
		; Doing it this way also makes divisions by 256 extremely fast compared to other divisions
		
		dec c
		jr z, division_by_one			; the algorithm below also breaks if Y = 1
		
		ld a, l
		ld l, e
		ld e, b			; e = 0

		; performs 4 iterations of the next loop at once if possible
		; a failed check costs 3 cycles
		; a successful check saves 14 cycles
		cp #0xF0
		jr c, 1$

		ld b, #0xE0
		add a
		add a
		add a
		add a
0$:
		rr b
1$:
		add a
		jr c, 0$
		
		ld h, a
		ld a, d

		; computes the high byte of the quotient and stores it in B
loop_b:
		rr h
		add h
		jr c, 0$
		sub h
0$:
		rl b
		jr c, loop_b
		
		ld c, #0xFE
		scf
ld_dh_ha:
		ld d, h
		ld h, a

		; computes the low byte of the quotient and stores it in C
loop_c:
		rr d
		rr e

		ld a, l
		add e
		ld a, h
		adc d
		jr nc, 0$
		add hl, de
0$:		
		rl c
		jr c, loop_c
ret_hl_in_de:
		ld d, h
		ld e, l
		ret
division_by_zero:
		scf
		; hl and bc are both expected to be 0
		jr ret_hl_in_de
division_by_one:
		ld b, d
		ld c, e
		; a is expected to be 0
		ld d, a
		ld e, a
		; the carry is expected to be clear
		ret
division_by_256:
		xor a			; clears the carry
		ld c, d
		ld d, a
		ld b, a
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
        xor b		; high byte of divisor
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
