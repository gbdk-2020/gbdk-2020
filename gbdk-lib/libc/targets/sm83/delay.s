	.include	"global.s"

	.area	_HOME

	;; Delay DE milliseconds
	;; 
	;; Entry conditions
	;;   DE = number of milliseconds to delay (1 to 65536, 0 = 65536)
	;; 
	;; Register used: AF, DE
	;; One millisecond last for 1048.576 M-cycles
	;; This has been rounded to 1048.500 M-cycles
loop:
	; each iteration of the loop lasts for 17.5 + 1031 = 1048.5 cycles on average
	call delay_1031
_delay::
.delay::
	dec de
	
	bit 0, e
	jr nz, 0$			; 2.5 cycles on average
0$:
	
	ld a, e
	or d
	jr nz, loop
	nop

delay_1031:
	; waits for 2 + 147 * 7 = 1031 cycles
	ld a, #147
1$:
	nop
	dec a
	ret z
	jr 1$
