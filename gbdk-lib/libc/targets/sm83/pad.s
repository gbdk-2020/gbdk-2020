	.include	"global.s"

	.area	_HOME

	;; Wait until all buttons have been released
.padup::
_waitpadup::
	PUSH	AF		; Save modified registers
	PUSH	DE
1$:
	LD	D,#0x7F		; wait for .jpad return zero 127 times in a row
2$:
	CALL	.jpad
	OR	A		; Have all buttons been released?
	JR	NZ,1$		; Not yet

	DEC	D
	JR	NZ,2$
	
	POP	DE
	POP	AF
	RET

	;; Get Keypad Button Status
	;; The following bits are set if pressed:
	;;   0x80 - Start   0x08 - Down
	;;   0x40 - Select  0x04 - Up
	;;   0x20 - B	    0x02 - Left
	;;   0x10 - A	    0x01 - Right
;
; Controller reading for Game Boy and Super Game Boy
;
; Copyright 2018, 2020 Damian Yerrick (adapted by Eldred Habert)
; 
; This software is provided 'as-is', without any express or implied
; warranty.  In no event will the authors be held liable for any damages
; arising from the use of this software.
; 
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
; 
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation would be
;    appreciated but is not required.
; 2. Altered source versions must be plainly marked as such, and must not be
;    misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source distribution.
;
_joypad::
.jpad::
	; Poll half the controller
	LD	A,#.P15
	CALL	.1$
	LD	E,A  ; B7-4 = 1; B3-0 = unpressed buttons

	; Poll the other half
	LD	A,#.P14
	CALL	.1$
	SWAP	A   ; A3-0 = unpressed directions; A7-4 = 1
	XOR	E    ; A = pressed buttons + directions
	LD	E,A   ; E = pressed buttons + directions

	; And release the controller
	LD	A,#(.P14 | .P15)
	LD	(.P1),A

	LD	A,E
	RET

1$:
	LDH	(.P1),A     ; switch the key matrix
	CALL	.2$  ; burn 10 cycles calling a known ret
	LDH	A,(.P1)     ; ignore value while waiting for the key matrix to settle
	LDH	A,(.P1)
	LDH	A,(.P1)     ; this read counts
	OR	$F0   ; A7-4 = 1; A3-0 = unpressed keys
2$:
	RET

	;; Wait for the key to be pressed
_waitpad::
	LD	D,A
	;; Wait for the key in D to be pressed
.wait_pad::
1$:
	CALL	.jpad		; Read pad
	AND	D		; Compare with mask?
	JR	Z,1$		; Loop if no intersection
	RET

