; REGTEMP stack for temporaries used by SDCC
; Keep this in sync with NUM_TEMP_REGS in mos6502/gen.c

	.area	_ZP (PAG)
REGTEMP::	.ds	8
