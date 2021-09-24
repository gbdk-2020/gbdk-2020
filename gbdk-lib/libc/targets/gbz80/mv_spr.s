	.include	"global.s"

	.area	_HOME

	;; Move sprite number C at XY = DE
.mv_sprite::
	LD	HL,#_shadow_OAM	; Calculate origin of sprite info
	SLA	C		; Multiply C by 4
	SLA	C
	LD	B,#0x00
	ADD	HL,BC

	LD	A,E		; Set Y
	LD	(HL+),A

	LD	A,D		; Set X
	LD	(HL+),A
	RET
