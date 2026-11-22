;/***************************************************************************
; *                                                                         *
; * Module  : rand.s                                                        *
; *                                                                         *
; * Purpose : A rand() generator using the linear congruential method       *
; *                                                                         *
; * Version : 1.01, January 7 1998                                          *
; *             Added _initrand to set seed without recompiling             *
; *           1, January 6 1998                                             *
; *                                                                         *
; * Author  : Luc Van den Borre ( Homepage : NOC.BASE.ORG )                 *
; *                                                                         *
; **************************************************************************/

	;; Why use an algorithm for generating random numbers?
	;;
	;; - Given a certain seed value, the same sequence of random numbers is generated
	;;   every time. This is a good thing when debugging (reproducible). On the other
	;;   hand, you've got 2^16 seed values, each of which will produce a sequence of
	;;   numbers that stays different for any of the other sequences for 'an appreciable
	;;   time.' (I can't say how long exactly.)
	;;
	;; - The linear congruential method is one of the 'best' random number generators
	;;   around. However, this implementation uses a 16 bit accumulator, while at least
	;;   32 bits are needed for a generator that passes all the statistical tests.
	;;   Still, I'm relatively confident that this is random enough for even the most
	;;   demanding game.
	;;
	;;   Compare this to getting random values from one of the hardware registers
	;;   (not reproducible, might not have all values). An array might be the best bet
	;;   if you don't need a lot of values (or have lots of memory spare),
	;;   or if you want values to be within a certain range.
	;;   And both would be faster than this. Also, this definitely isn't the fastest
	;;   algorithm I know, and certainly for games less strict algorithms might be
	;;   appropriate (shift and xor ?).
	;;   It's your choice - but if you're doing Monte Carlo physics simulations on the
	;;   GameBoy, this is a safe bet!

	.area	_DATA
___rand_seed::
.randlo:		; Storage for last random number (or seed)
	.ds	0x01
.randhi:
	.ds	0x01

	.area	_HOME

	;; Random number generator using the linear congruential method
	;;  X(n+1) = (a*X(n)+c) mod m
	;; with a = 17, m = 65536 and c = $5c93 (arbitrarily)
	;; The seed value is also chosen arbitrarily as $a27e
	;; Ref : D. E. Knuth, "The Art of Computer Programming" , Volume 2
	;;
	;; Exit conditions
	;;   DE = Random number [0,2^16-1]
	;;
	;; Registers used:
	;;   A, HL (need not be saved) and DE (return register)
	;;

_rand::				; Banked
_randw::			; Banked
	ld hl, #.randlo
	ld a, (hl+)
	ld e, a
	ld d, (hl)		; D = randhi

	; HL = 17 * DE + 0x5C93
	ld h, d
	ld l, e

	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl
	add hl, de
	ld de, #0x5C93
	add hl, de
	
	ld d, l
	ld e, h			; de = return value

	ld hl, #.randlo
	ld a, d
	ld (hl+), a
	ld (hl), e
	
	;; Note D is the low byte,E the high byte. This is intentional because
	;; the high byte can be slightly 'more random' than the low byte, and I presume
	;; most will cast the return value to a uint8_t. As if someone will use this, tha!
	ret

	;; This sets the seed value. Call it whenever you like
	;;
	;; Exit conditions
	;;   None
	;;
	;; Registers used:
	;;   A, HL (need not be saved) and DE (return register)
	;;

_initrand::
	LDA	HL,2(SP)
.initrand::
	LD	A,(HL+)
	LD	(.randlo),A
	LD	A,(HL)
	LD	(.randhi),A
	RET
