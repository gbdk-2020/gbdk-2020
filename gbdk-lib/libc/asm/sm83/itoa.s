;--------------------------------------------------------------------------
;  itoa.s
;
;  Copyright (c) 2026, Phidias618
;
;--------------------------------------------------------------------------

        .module itoa

        .area   _HOME



;        push    BC
;        ldhl    sp, #4
;        ld      A, (HL+)
;        ld      E, A
;        ld      A, (HL+)
;        ld      D, A            ; DE: uint
;        ld      A, (HL+)
;        ld      H, (HL)         ; HL: dest
;        ld      L, A
;        call    utoa_hl
;        pop     BC
;        ret

_itoa::
        pop hl                ; get return address
        pop de                ; get input number
        pop bc                ; get destination string
        add sp, #-4
        push hl               ; put the return address back

;        push    BC
;        ldhl    sp, #4
;        ld      A, (HL+)
;        ld      E, A
;        ld      A, (HL+)
;        ld      D, A            ; DE: int
;        ld      A, (HL+)
;        ld      C, A
;        ld      B, (HL)         ; BC: dest
;        call    .itoa
;        pop     BC
;        ret
        
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
        pop hl                ; get return address
        pop de                ; get input number
        pop bc                ; get destination string
        add sp, #-4
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
	ld a, e
	
	ld b, e
	ld c, d
	
.rept 4
	srl d
	rra
.endm
	ld l, a
	ld h, d
	srl d
	rra
	ld e, a
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
	
	pop bc
        ld d, b
        ld e, c                        ; set the return value
	
	; reverse the order of the string
	ld a, l
	sub c                           ; a = length-1
	ret z			        ; no need to reverse if length == 1
	cp #3
5$:
	; swap 2 chars
	ld d, (hl)
	ld a, (bc)
	ld (hl-), a
	ld a, d
	ld (bc), a
	
	ret c
	inc bc
	scf
	jr 5$			        ; swap 2 more chars
