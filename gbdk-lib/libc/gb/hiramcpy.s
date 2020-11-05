	.include	"global.s"

	;; BANKED:	checked
	.area	_BASE

_hiramcpy::
	LDA	HL,4(SP)	; Skip return address and registers
	LD	E,(HL)		; E = dst
	LDA	HL,7(SP)
	LD	A,(HL-)
	LD	D, A		; D = n
	LD	A,(HL-)		; HL = src
	LD	L,(HL)
	LD	H,A

	;; Copy memory zone to HIRAM
	;; 
	;; Entry conditions
	;;   E = destination
	;;   D = length
	;;   HL = source
	;; 
	;; Preserves: BC
.hiramcpy::
	LD	A,E
	LD	E,C
	LD	C,A
1$:
	LD	A,(HL+)
	LDH	(C),A
	INC	C
	DEC	D
	JR	NZ,1$
	LD	C,E
	RET
