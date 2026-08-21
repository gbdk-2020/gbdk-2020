	.include	"global.s"

	.title	"display off"
	.module	Display

	.area	_HOME

.display_off::
_display_off::
        ;; Check if the screen is on
        LDH     A, (rLCDC)
        AND     #LCDCF_ON
        RET     Z                    ; Return if screen is off
1$:                                  ; We wait for the *NEXT* VBL
        LDH     A, (rLY)
        CP      #0x92                ; Smaller than or equal to 0x91?
        JR      NC,1$                ; Loop until smaller than or equal to 0x91
2$:
        LDH     A, (rLY)
        CP      #0x91                ; Bigger than 0x90?
        JR      C, 2$                ; Loop until bigger than 0x90

        LDH     A, (rLCDC)
        AND     #~LCDCF_ON
        LDH     (rLCDC),A            ; Turn off screen
        RET
