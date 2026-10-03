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
        
        ld a, #'-'
        ld (bc), A
        inc	bc
        
        call    .utoa
        dec     bc
        ret

_uitoa::
    pop hl                ; get the return address
    inc sp				  ; get rid of the unused radix argument
    push hl               ; put the return address back
.utoa::                         ; convert unsigned int into ascii
    ; input :
	;  - de : the 16 bit unsigned number to convert to ascii
	;  - bc (dst) : the pointer of the destination string
	; bc is preserved by this function
	
	ld a, #'0'
	push bc				; save dst

	ld hl, #-10000		
	add hl, de
	jr c, 0$			; 5 digit number ?
	
	ld hl, #-1000
	add hl, de
	jr c, 2$			; 4 digit number ?
	
	ld hl, #-100
	add hl, de
	jr c, 4$			; 3 digit number ?
	
	pop hl 
	ld a, e
	
	ld de, #2561 + '0'	; ld d, #10; ld e, #'0'+1
	
	sub d				; sub #10
	jr nc, 7$			; 2 digit number ?
	jr 8$				; 1 digit number	
0$:
	ld de, #-10000
1$:
	inc a
	add hl, de
	jr c, 1$
	
	ld (bc), a
	inc bc
	
	ld de, #10000
	add hl, de
	ld a, #'0'-1
2$:
	ld de, #-1000
3$:
	inc a
	add hl, de
	jr c, 3$
	
	ld (bc), a
	inc bc
	
	ld de, #1000
	add hl, de
	ld a, #'0'-1
4$:
	ld de, #-100
5$:
	inc a
	add hl, de
	jr c, 5$
	
	ld h, b
	ld b, l
	ld l, c
	
	ld (hl+), a
	
	ld a, b
	sub e				; sub #-100
	
	pop bc				; restore dst to return it
	
	ld de, #2559 + '0'	; ld d, #10; ld e, #'0' - 1
6$:
	inc e
7$:
	sub d				; sub #10
	jr nc, 6$
	
	ld (hl), e
	inc hl
8$:
	add #'0' + 10
	ld (hl+), a
	ld (hl), #0
	
	ret
