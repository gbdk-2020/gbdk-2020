# sm83 specific Makefile

TOPDIR = ../../..

THIS = mos6502

ASSRC = __sdcc_indirect_jsr.s _memcpy.s _strcpy.s _strcmp.s \
	_divuint.s _divsint.s _modsint.s _moduint.s \
	_divulong.s _divslong.s _modulong.s _modslong.s \
	_mulint.s \
	_muluchar.s _mulschar.s

CSRC =	_memmove.c _memset.c _ret.c abs.c \
	_rrulonglong.c _rrslonglong.c \
	atomic_flag_test_and_set.c

include $(TOPDIR)/Makefile.common

AS = $(AS_6500)

include ../Makefile.port


