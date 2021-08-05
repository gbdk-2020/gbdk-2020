# GB specific Makefile

TOPDIR = ../../../..

THIS = sms
PORT = z80

CSRC = 

ASSRC =	outi.s vmemcpy.s \
	set_data.s set_2bpp_data.s \
	pad.s pad_ex.s \
	int.s nmi.s \
	__sdcc_bcall.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
