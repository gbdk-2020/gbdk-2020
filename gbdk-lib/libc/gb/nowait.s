	;; interrupt handler that does not wait for .STAT
	;; must be the last one in chain
_nowait_int_handler::
	ADD	SP,#4

	POP	DE 
	POP	BC
	POP	HL 
	POP	AF
	RETI

_wait_int_handler::    
	ADD	SP,#4
	POP	DE 
	POP	BC
	POP	HL 
1$:
	LDH	A,(#_STAT_REG)
	AND	#0X02
	JR	Z,1$
2$:
	LDH	A, (#_STAT_REG)
	AND	#0X02
	JR	NZ, 2$
	POP	AF
	RETI
