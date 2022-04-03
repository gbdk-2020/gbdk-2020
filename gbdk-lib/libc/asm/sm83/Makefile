# sm83 specific Makefile

TOPDIR = ../../..

THIS = sm83

ASSRC = __sdcc_call_hl.s abs.s mul.s div.s shift.s crt0_rle.s \
	itoa.s strlen.s reverse.s labs.s ltoa.s \
	setjmp.s atomic_flag_test_and_set.s \
	memcpy.s _memset.s _strcmp.s _strcpy.s _memcmp.s \
	rand.s arand.s \
	bcd.s

CSRC =	_memmove.c

include $(TOPDIR)/Makefile.common

AS = $(AS_SM83)

include ../Makefile.port


