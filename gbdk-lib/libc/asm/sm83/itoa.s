;--------------------------------------------------------------------------
;  itoa.s
;
;  Copyright (c) 2026, Phidias618
;
;--------------------------------------------------------------------------

        .module itoa

        .area   _HOME


_itoa::
        pop hl                ; get the return address
        inc sp				  ; get rid of the unused radix argument
        push hl               ; put the return address back
.itoa::                         ; convert int into ascii
        bit 7, d
        jr z, .utoa

        ; DE = -DE
        xor a
        sub e
        ld e, a
        sbc a
        sub d
        ld d, a
        
        ld      A, #'-'
        ld      (BC), A
        inc     BC
        
        call    .utoa
        dec     DE
        ret

_uitoa::
    pop hl                ; get the return address
    inc sp				  ; get rid of the unused radix argument
    push hl               ; put the return address back
.utoa::                         ; convert unsigned int into ascii
    ; input :
	;  - de (x) : the 16 bit unsigned number to convert to ascii
	;  - bc (dst) : the pointer to the destination string

    ld h, b
	ld l, c

	push bc
	ld bc, #10
	jr 4$
0$:
	push hl
	
	; compute an approximation of x / 10
	; by using the approximation 1/10 ~ 0.0001100110011
	
	ld b, e
	ld c, d

	ld l, e
	ld h, d
	xor a
.rept 3
	add hl, hl
	adc a
.endm
	ld e, h
	ld d, a	
	
	add hl, hl
	adc a
	ld l, h
	ld h, a
	add hl, de
	
	ld e, b
	xor a
	ld b, a
	
	add hl, bc
	srl c
	add hl, bc

	srl c
	srl c
	srl c
	add hl, bc
	srl c
	add hl, bc
	
	; computes an approximation of the remainder 
	; by using the computed approximation of x / 10
	; it will always be in the range [0; 75]
	sub l
	add a
	add a
	sub l
	add a
	add e				; e = x - 10 * hl

	; fix both the quotient and the remainder of x / 10
	; using the fact that the remainder should be less than 10
	cp #40
	jr c, 1$
	sub #40
	ld c, #4
	add hl, bc			; add hl, #4
1$:
	cp #20
	jr c, 2$
	sub #20
	; this can *not* trigger an OAM corruption glitch
	; because hl is too small
	inc hl
	inc hl
2$:
	ld c, #10
	cp c				; cp #10
	jr c, 3$
	sub c				; sub #10
	; this can *not* trigger an OAM corruption glitch
	; because hl is too small
	inc hl
3$:
	; now a = x % 10
	ld d, h
	ld e, l				; x = x / 10
	
	pop hl
	add #'0'
	ld (hl+), a
4$:
	ld a, e
	sub c				; sub #10
	ld a, d
	sbc b				; sbc #0

	jr nc, 0$			; exit the loop if x is only one digit long
	
	ld a, e
	add #'0'
	ld (hl+), a
	
	xor a
	ld (hl-), a                     ; store the terminator of the string
	
	pop bc							; bc = dst
	
	; reverse the order of the string
	ld a, l
	sub c                           ; a = length-1
	ret z			        		; no need to reverse if length == 1
	cp #3

	; swap 2 chars
	ld d, (hl)
	ld a, (bc)
	ld (hl-), a
	ld a, d
	ld (bc), a
	
	ret c
	inc bc

	; swap 2 more chars
	ld d, (hl)
	ld a, (bc)
	ld (hl-), a
	ld a, d
	ld (bc), a

	dec bc							; bc = dst
	ret
