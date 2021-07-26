	.include	"global.s"

	;; BANKED:	checked, imperfect
	.area	_BASE

	;; Set sprite number C to tile D
.set_sprite_tile::
	LD	HL,#_shadow_OAM+2	; Calculate origin of sprite info

	SLA	C		; Multiply C by 4
	SLA	C
	LD	B,#0x00
	ADD	HL,BC

	LD	A,D		; Set sprite number
	LD	(HL),A
	RET
