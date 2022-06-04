# z80 specific Makefile

TOPDIR = ../../..

THIS = z80

ASSRC = __strreverse.s strcpy.s strlen.s \
	divmixed.s divsigned.s divunsigned.s \
	modsigned.s modunsigned.s modmixed.s \
	mul.s mulchar.s \
	__itoa.s __ltoa.s \
	__uitobcd.s __ultobcd.s \
	memcpy.s memmove.s memset.s memcmp.s \
	setjmp.s \
	abs.s \
	rand.s arand.s \
	__sdcc_call_hl.s __sdcc_call_iy.s \
	atomic_flag_test_and_set.s __sdcc_critical.s \
	crtenter.s \
	../f_ibm_full.s ../f_ibm_sh.s ../f_italic.s ../f_min.s ../f_spect.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../Makefile.port


