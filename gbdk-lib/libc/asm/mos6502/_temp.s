; REGTEMP stack for temporaries used by SDCC
; Keep this in sync with NUM_TEMP_REGS in mos6502/gen.c
;
; Note: This has been patched by GBDK, to include variables in _ret01.s, _ret23.s, _ret4567.s
; This is to ensure a continuous section of temporaries to simplify interrupt handling.
;
	.area	_ZP (PAG)
ALL_REGTEMPS_BEGIN::
DPTR::  .ds     2
REGTEMP::	.ds	8
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
___SDCC_m6502_ret6::
        .ds 1
___SDCC_m6502_ret7::
        .ds 1
ALL_REGTEMPS_END::
