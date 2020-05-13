	.include	"global.s"

	;; ****************************************
	;; Beginning of module
	;; BANKED: checked
	.title	"BGB_emu"
	.module	BGB_emu
	.area	_EMU_HEADER (ABS)

	.org	0x08
	JP	_BGB_profiler_message

	.area	_HOME
    
	;; BGB profiler message
_BGB_profiler_message::
	LD	D, D
	JR	1$
	.dw	0x6464
	.dw	0
	.ascii "PROFILE,%(SP+$0)%,%(SP+$1)%,%A%,%TOTALCLKS%,%ROMBANK%,%WRAMBANK%"
1$:	RET
