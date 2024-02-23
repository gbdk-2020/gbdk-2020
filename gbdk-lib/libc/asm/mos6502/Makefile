# sm83 specific Makefile

TOPDIR = ../../..

THIS = mos6502

ASSRC = __sdcc_indirect_jsr.s _memcpy.s _strcpy.s _strcmp.s _memset.s \
	rand.s \
	_divuint.s _divsint.s _modsint.s _moduint.s _moduchar.s \
	_divulong.s _divslong.s _modulong.s _modslong.s \
	_divuchar.s \
	_mulint.s \
	_mullong.s \
	_muluchar.s _mulschar.s \
	_setjmp.s \
	_dptr.s _ret01.s _ret23.s _ret4567.s _temp.s

CSRC =	_memmove.c _memset.c abs.c \
	_rrulonglong.c _rrslonglong.c \
	atomic_flag_test_and_set.c \
	__itoa.c _strlen.c

include $(TOPDIR)/Makefile.common

AS = $(AS_6500)

include ../Makefile.port


