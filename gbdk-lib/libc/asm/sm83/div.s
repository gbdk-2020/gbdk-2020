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


	    ; stores -Y in hl and check that -Y is not zero
        xor a
		sub c
		ld l, a
		sbc a
		sub b
		ld h, a
		
		or l
        jr z, .division_by_zero


		inc h
		jr z, 0$
		dec h
		xor a
		;ld b, a
4$:
		rra
		add hl, hl
		jr c, 4$
		;ld c, a

		;;ld bc, hl
		;;ld hl, de

		;;ld e, a
		;;ld d, #0
		
		;ld a, h
		;ld h, d
		;ld d, a

		;ld a, l
		;ld l, e
		;ld e, a
		
		jr last_loop
0$:
		dec c
		jr z, ...
		
		ld c, h			; c = 0
		ld a, l
		
		;;cp #0xF0
		;;jr c, 10$
		;;ld c, #0xF0
		;;swap a
		;;and c
10$:
		;;ccf
1$:
		rr c
		add a
		jr c, 1$

		;;ld l, a
		;ld c, a
		ld a, d
2$:
		rr l
		add l
		jr c, 3$
		sub l
3$:
		rl c
		jr c, 2$

		ld h, a
		ld d, l
		ld l, b		; l = 0

		ld b, c
		ld c, #0xFE
		
last_loop:
		rr d
		rr e

		; 7/8
		ld a, l
		add e
		ld a, h
		adc d
		jr nc, 0$
		add hl, de
0$:		
		rl c
		jr c, last_loop

		ld d, h
		ld e, l
		ret

__divuint::
        ; computes the quotient and the remainder of X / Y
        ; X is stored in de
        ; Y is stored in bc
        ; outputs the quotient in bc
        ; outputs the remainder in de
        ; if Y = 0 then the carry is set and quotient = 0 and remainder = 0
        ; otherwise the carry is cleared
		
        
        ; stores -Y in hl and check that -Y is not zero
        xor a
		sub c
		ld l, a
		sbc a
		sub b
		ld h, a
		
		or l
        jr z, .division_by_zero

		; computes a large K such that (Y << K) is still a 16 bit number
		;   K will be stored in BC as a base 1 number
		;   Y << (K + 1) will be stored in HL and the carry will be used to store its 17th bit
		
		ld a, h
		inc a
		jr z, 1$
		xor a
		ld c, a
0$:
		rra
		add hl, hl
		jr c, 0$
		ld b, a
		
		jr 3$
1$:
		ld h, l
		ld l, a			; l = 0
		ld c, a			; c = 0
		ld b, #0xFF
		
    	
		ccf
		jr c, 3$

		ld a, h
		cp #0xF0
		jr c, 11$
		ld c, #0xF0
		swap a
		and c
		ld h, a
11$:
		scf
2$:
		rr c
		add a
		jr c, 2$

		ld h, a
3$:	
        ; swaps the content of hl and de
        ld a, h
        ld h, d
        ld d, a
        
        ld a, l
        ld l, e
        ld e, a
		
10$:
		rr d
		rr e

		sla c
		rl  b
		
		ld a, e 
        add l
        ld a, d
        adc h
		jr nc, 10$
11$:
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
4$:
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
        
        jr nc, 5$
        add hl, de
5$:
        ; fill bc with one bit of the result and decrements K at the same time
        rl c
        rl b
        
        jr c, 4$
ret_hl_in_de:
        ld d, h
        ld e, l
        ; status of the registers
        ; bc = quotient
        ; de = remainder
        ; carry = 0 if Y != 0, 1 if Y == 0
        
        ret
.division_by_zero:
        ; returns both a quotient of 0 and a remainder of 0
        ; if this is reached, then bc = 0
        scf
		jr ret_hl_in_de


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
