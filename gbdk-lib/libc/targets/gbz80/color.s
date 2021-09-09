	.include        "global.s"

	;; Data
	.area	_DATA
	;; Foreground drawing colour
.fg_colour::	
	.ds	1
	;; Background drawing colour
.bg_colour::	
	.ds	1
	;; Drawing mode (.SOILD etc)
.draw_mode::
	.ds	1

	.area _HOME
_color::			
	LDA	HL,2(SP)	; Skip return address and registers
	LD	A,(HL+)	        ; A = Foreground
	LD	(.fg_colour),a
	LD	A,(HL+)
	LD	(.bg_colour),a
	LD	A,(HL)
	LD	(.draw_mode),a
	RET
