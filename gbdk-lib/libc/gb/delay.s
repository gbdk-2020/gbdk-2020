	.include	"global.s"

	;; BANKED:	checked, imperfect
	.area	_BASE

	;; Delay DE milliseconds
	;; 
	;; Entry conditions
	;;   DE = number of milliseconds to delay (1 to 65536, 0 = 65536)
	;; 
	;; Register used: AF, DE
	.CPMS	= 4194/4	; 4.194304 MHz

_delay::
	LDA	HL,2(SP)	; Skip return address
	LD	E,(HL)		; DE = delay
	INC	HL
	LD	D,(HL)

.delay::			; 6 cycles for the CALL
	PUSH	BC		; 4 cycles
	CALL	.dly		; 12 cycles to return from .dly (6+1+5)
	LD	B,#.CPMS/20-2	; 2 cycles
				; =========
				; 24 cycles
.ldlp:
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	NOP			; 1 cycles
5$:	DEC	B		; 1 cycles
	JP	NZ,.ldlp	; 3 cycles (if TRUE: 4 cycles)
	NOP			; 1 cycles
				; =========
				; 20 cycles
	;; Exit in 16 cycles
	POP	BC		; 3 cycles
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	NOP			; 1 cycles
	NOP			; 1 cycles
	RET			; 4 cycles
				; =========
				; 16 cycles

	;; Delay all but last millisecond
.dly:
	DEC	DE		; 2 cycles
	LD	A,E		; 1 cycles
	OR	D		; 1 cycles
	RET	Z		; 2 cycles (upon return: 5 cycles)
	LD	B,#.CPMS/20-1	; 2 cycles
				; =========
				; 8 cycles
.dlp:
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	NOP			; 1 cycles
	DEC	B		; 1 cycles
	JP	NZ,.dlp		; 3 cycles (if TRUE: 4 cycles)
	NOP			; 1 cycles
				; =========
				; 20 cycles
	;; Exit in 15 cycles
	PUSH	BC		; 4 cycles
	POP	BC		; 3 cycles
	NOP			; 1 cycles
	NOP			; 1 cycles
	JR	.dly		; 3 cycles
				; =========
				; 12 cycles
