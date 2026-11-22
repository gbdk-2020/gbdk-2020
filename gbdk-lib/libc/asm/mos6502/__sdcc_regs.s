; PSEUDO registers used by SDCC codegen
; REGTEMP stack for temporaries
; Keep this in sync with NUM_TEMP_REGS in mos6502/gen.c
; DPTR used for dereferencing pointers
; ___SDCC_m6502_retX used for return values of size>2

	.module __sdcc_regs

	.area	_ZP (PAG)
ALL_REGTEMPS_BEGIN::
REGTEMP::
___SDCC_m6502_ret0::
        .ds 1
___SDCC_m6502_ret1::
        .ds 1
___SDCC_m6502_ret2::
        .ds 1
___SDCC_m6502_ret3::
        .ds 1
___SDCC_m6502_ret4::
        .ds 1
___SDCC_m6502_ret5::
        .ds 1
DPTR::
___SDCC_m6502_ret6::
        .ds 1
___SDCC_m6502_ret7::
        .ds 1
ALL_REGTEMPS_END::
