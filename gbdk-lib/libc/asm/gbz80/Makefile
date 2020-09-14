# gbz80 specific Makefile

TOPDIR = ../../..

THIS = gbz80

ASSRC = __sdcc_call_hl.s abs.s mul.s div.s shift.s crt0_rle.s \
	itoa.s strlen.s reverse.s labs.s ltoa.s \
	setjmp.s atomic_flag_test_and_set.s \
	_memcpy.s _memset.s _strcmp.s _strcpy.s

include $(TOPDIR)/Makefile.common

AS = $(AS_GBZ80)

include ../Makefile.port


