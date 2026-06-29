	.include	"global.s"

	.area	_HOME

	;; Delay DE milliseconds
	;; 
	;; Entry conditions
	;;   DE = number of milliseconds to delay (1 to 65536, 0 = 65536)
	;; 
	;; Register used: AF, DE
	;;
	;; One millisecond last for 1048.576 M-cycles on normal speed mode
	;; This has been rounded to 1048.000 M-cycles
	;; One millisecond last for 2097.152 M-cycles on double speed mode
	;; This has been rounded to 2097.000 M-cycles
loop:
	; on normal speed mode each iteration lasts for 6 + 1028 + 3 + 1 + 3 + 2 + 2 + 3 = 1048 cycles
	; on double speed mode each iteration lasts for 6 + 3 + 1 + 6 + 1046 + 2 + 1 + 1 + 3 = 2097 cycles
	call delay_1028
_delay::
.delay::

	ldh a, (rKEY1)
	add a
	call c, delay_1046

	dec de
	
	ld a, e
	or d
	jr nz, loop
	nop

delay_1028:
	; waits for 2 + 147 * 7 = 1031 cycles
	ld a, #171
L0:
	dec a
	ret z
L1:
	jr L0
delay_1046:
	; waits for 2 + 3 + 3 + 6 * 173 = 1046 cycles
	ld a, #173
	jr L1
